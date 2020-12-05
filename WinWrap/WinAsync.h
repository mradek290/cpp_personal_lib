
#ifndef __WIN_WRAP_ASYNC_HEADER__
#define __WIN_WRAP_ASYNC_HEADER__

#include "../x0/meta.h"
#include "../x0/opt.h"

#include <windows.h>

#include "../x0/refwrapper.h"
#include "../x0/closure.h"
#include "../x0/invocation.h"

namespace WinWrap{

struct WinSectionError : public x0::Error{
    using x0::Error::Error;
};

class WinSection : protected x0::LockedClass, private CRITICAL_SECTION{

    friend class WinPass;

public:

    WinSection(){
        InitializeCriticalSection(this);
    }

    ~WinSection(){
        DeleteCriticalSection(this);
    }
};

//-----------------------------------------------------------

class WinPass : protected x0::LockedClass {

    WinSection* section;

public:

    WinPass() :
        section{0}
    {
    }

    WinPass( WinSection& sec )
    {
        section = 0;
        EnterCriticalSection(&sec);
        section = &sec;
    }

    bool isActive() const {
        return section;
    }

    void acquire( WinSection& sec ){

        if( isActive() ){
            throw( WinSectionError(
                    "Attempting to re-use a critical seciton pass without relinquishing it.",
                    x0::Error::CRITICAL
                )
            );
        }
        
        section = &sec;
        EnterCriticalSection(&sec);
    }

    bool acquireAsync( WinSection& sec ){

        if( !isActive() && TryEnterCriticalSection(&sec) ){

            section = &sec;
            return true;
        }

        return false;
    }

    void relinquish(){

        if( isActive() ){

            LeaveCriticalSection(section);
            section = 0;
        }
    }

    ~WinPass(){
        relinquish();
    }
};


///////////////////////////////////////////////////////////////////////////

struct WinTimerError : public x0::Error{
    using x0::Error::Error;
};

class WinTimer : protected x0::LockedClass {

public:

    typedef LONGLONG tick_t;

    static constexpr tick_t TimeToTicks( double x ){
        return  x < 0 ?
                static_cast<tick_t>( x*1E7 ) :
                static_cast<tick_t>( -x*1E7);
    }

    static constexpr double TicksToTime( tick_t t ){
        return  t < 0 ? t / -1E7 : 0;
    }

private:

    HANDLE w_timer;
    LARGE_INTEGER ticks;
    bool active;

    static void switch_off( void* vp, DWORD, DWORD ){
        *static_cast<bool*>(vp) = false;
    }

public:    

    WinTimer() :
        w_timer{ CreateWaitableTimer( 0, true, 0) },
        active{ false }
    {
        if( !w_timer )
            throw( WinTimerError(
                    "Unable to create timer system resource.",
                    x0::Error::CRITICAL,
                    GetLastError() 
                )
            );
    }

    WinTimer( double x ) :
        WinTimer()
    {
        ticks.QuadPart = TimeToTicks(x);
        active = SetWaitableTimer( w_timer, &ticks, 0, switch_off, &active, 0 );
        if( !active )
            throw(
                WinTimerError(
                    "Setup of system timer resource failed.",
                    x0::Error::CRITICAL,
                    GetLastError()
                )
            );
    }

    ~WinTimer(){
        abort();
        CloseHandle(w_timer);
    }

    void abort(){

        if( active ){
            LARGE_INTEGER z;
            z.QuadPart = 0;

            CancelWaitableTimer(w_timer);
            SetWaitableTimer( w_timer, &z, 0, 0, 0, 0 );
            WaitForSingleObject( w_timer, INFINITE );
        }
        active = false;
    }

    bool join(){
        return WaitForSingleObject(w_timer, INFINITE) == WAIT_OBJECT_0;
    }

    bool launch( double x ){

        if( !active ){

            ticks.QuadPart = TimeToTicks(x);
            active = SetWaitableTimer( w_timer, &ticks, 0, switch_off, &active, 0 );
        }

        return active;
    }

    bool launch_and_join( double x ){
        launch(x);
        return join();
    }

    double getOriginalDuration() const {
        return TicksToTime(ticks.QuadPart);
    }

    bool isActive() const {
        return active;
    }
};

//---------------------------------------------------------

struct WinEventError : public x0::Error{
    using x0::Error::Error;
};

class WinEvent : protected x0::LockedClass {

    void* w_event;

public:

    WinEvent( bool manreset = false ) :
        w_event( CreateEvent( 0, manreset, false, 0 ) )
    {
        if( !w_event ){
            throw( WinEventError(
                    "Failed to create event",
                    x0::Error::CRITICAL,
                    GetLastError()
                )
            );
        }
    }

    ~WinEvent(){
        CloseHandle( w_event );
    }

    void raise(){

        if( SetEvent(w_event) == 0 ){
            throw( WinEventError(
                    "Unable to signal event",
                    x0::Error::CRITICAL,
                    GetLastError()
                )
            );
        }
    }

    void reset(){

        if( ResetEvent(w_event) == 0 ){
            throw( WinEventError(
                    "Unable to reset event",
                    x0::Error::CRITICAL,
                    GetLastError()
                )
            );
        }
    }

    void await(){

        if( WaitForSingleObject( w_event, INFINITE ) == WAIT_FAILED ){
            throw( WinEventError(
                    "Unable to await event",
                    x0::Error::CRITICAL,
                    GetLastError()
                )
            );
        }
    }
};

///////////////////////////////////////////////////////////////////////////

unsigned long GetLogicalCoreCount(){

    SYSTEM_INFO nfo;
    GetSystemInfo(&nfo);
    return nfo.dwNumberOfProcessors;
}

struct   WinInvocationStyle {};
struct : WinInvocationStyle {} Async;
struct : WinInvocationStyle {} AsyncNew;
struct : WinInvocationStyle {} AsyncManual;
struct : WinInvocationStyle {} Deferred;
struct : WinInvocationStyle {} DeferredNew;
struct : WinInvocationStyle {} DeferredManual;

struct WinThreadError : public x0::Error{
    using x0::Error::Error;
};

template< class lambda_t, class closure_t >
class WinThread : protected x0::LockedClass, private x0::Invocation<lambda_t,closure_t> {

    typedef x0::Invocation<lambda_t,closure_t> super;

public:

    typedef typename super::return_t return_t;
    enum state_t : unsigned char {
        STANDBY,
        RUNNING,
        SUSPENDED,
        FINISHED,
        TERMINATED,
        DEAD
    };

    template< class T0, class T1, class ... Tn >
    friend auto WinAsync( T0, T1&&, Tn&& ... );

private:

    void* w_thread;
    DWORD w_id;
    bool  auto_cleanup;
    state_t state;
    WinEvent finish_signal;
    x0::Opt<return_t> retval;

    static DWORD WINAPI callback( void* vp ){

        WinThread& thread = *reinterpret_cast<WinThread*>(vp);

        thread.state = RUNNING;
        if constexpr( !x0::IsSameType<return_t,void> )
            thread.retval = thread.invoke();
        else
            thread.invoke();
        thread.state = FINISHED;
        thread.finish_signal.raise();

        if( thread.auto_cleanup ){
            delete &thread;
        }

        return 0;
    }

    WinThread( bool autoclean, DWORD ws, lambda_t&& fn, closure_t&& args ) :
        super{
            x0::PreserveArg<lambda_t>(fn),
            x0::PreserveArg<closure_t>(args)
        },
        auto_cleanup{ autoclean },
        state{ STANDBY },
        finish_signal{ true },
        retval{}
    {
        w_thread = CreateThread( 
            /*default security*/ 0,
            /*default stack sz*/ 0,
            callback,
            this,
            ws,
            &w_id
        );

        if( !w_thread )
            throw( WinThreadError(
                "Unable to create thread.",
                x0::Error::FATAL,
                GetLastError()
            )
        );
    }

    WinThread() = delete;

public:

    void await(){
        finish_signal.await();
    }

    void suspend(){

        if( state == RUNNING ){
            if( SuspendThread( w_thread ) == static_cast<DWORD>(~0) ){
                throw( WinThreadError(
                        "Unable to suspend thread execution.",
                        x0::Error::CRITICAL,
                        GetLastError()
                    )
                );
            }else
                state = SUSPENDED;
        }
    }

    void resume(){

        if( state == SUSPENDED || state == STANDBY ){
            if( ResumeThread( w_thread ) == static_cast<DWORD>(~0) ){
                throw( WinThreadError(
                        "Unable to resume thread execution.",
                        x0::Error::CRITICAL,
                        GetLastError()
                    )
                );
            }else
                state = RUNNING;
        }
    }

    void join(){
        resume();
        await();
    }

    void terminate(){

        if( w_thread ){

            if( state != FINISHED ){
                DWORD exc;
                GetExitCodeThread( w_thread, &exc );
                TerminateThread( w_thread, exc );
            }

            CloseHandle(w_thread);
            w_thread = 0;
            state = TERMINATED;
        }

        finish_signal.raise();
    }

    unsigned long getID() const{
        return w_id;
    }

    state_t getState() const {
        return state;
    }

    auto& getReturnValue(){
        await();
        return retval.getValue();
    }

    x0::Opt<return_t>& peekReturnValue(){
        return retval;
    }

    ~WinThread(){
        terminate();
        state = DEAD;
    }
};

template< class style, class lambda_t, class ... Tn >
auto WinAsync( style, lambda_t&& fn, Tn&& ... argn ){

    static_assert(
        !x0::IsSameType<style,WinInvocationStyle> && x0::HasImplicitConversion<style,WinInvocationStyle>,
        "Attempting to invoke thread with invalid style."
    );

    if constexpr( x0::IsSameType<x0::RemoveCRef<style>, decltype(Async)> )
        return new WinThread{ true, 0,
            x0::X0_CXX_wlfi(
                x0::PreserveArg<lambda_t>(fn),
                x0::PreserveArg<decltype(argn)>(argn) ...
            ),
            x0::X0_CXX_wafi(
                x0::PreserveArg<decltype(argn)>(argn) ...
            )
        };

    if constexpr( x0::IsSameType<x0::RemoveCRef<style>, decltype(AsyncNew)> )
        return new WinThread{ false, 0,
            x0::X0_CXX_wlfi(
                x0::PreserveArg<lambda_t>(fn),
                x0::PreserveArg<decltype(argn)>(argn) ...
            ),
            x0::X0_CXX_wafi(
                x0::PreserveArg<decltype(argn)>(argn) ...
            )
        };

    if constexpr( x0::IsSameType<x0::RemoveCRef<style>, decltype(AsyncManual)> )
        return WinThread{ false, 0,
            x0::X0_CXX_wlfi(
                x0::PreserveArg<lambda_t>(fn),
                x0::PreserveArg<decltype(argn)>(argn) ...
            ),
            x0::X0_CXX_wafi(
                x0::PreserveArg<decltype(argn)>(argn) ...
            )
        };

    if constexpr( x0::IsSameType<x0::RemoveCRef<style>, decltype(Deferred)> )
        return new WinThread{ true, CREATE_SUSPENDED,
            x0::X0_CXX_wlfi(
                x0::PreserveArg<lambda_t>(fn),
                x0::PreserveArg<decltype(argn)>(argn) ...
            ),
            x0::X0_CXX_wafi(
                x0::PreserveArg<decltype(argn)>(argn) ...
            )
        };

    if constexpr( x0::IsSameType<x0::RemoveCRef<style>, decltype(DeferredNew)> )
        return new WinThread{ false, CREATE_SUSPENDED,
            x0::X0_CXX_wlfi(
                x0::PreserveArg<lambda_t>(fn),
                x0::PreserveArg<decltype(argn)>(argn) ...
            ),
            x0::X0_CXX_wafi(
                x0::PreserveArg<decltype(argn)>(argn) ...
            )
        };

    if constexpr( x0::IsSameType<x0::RemoveCRef<style>, decltype(DeferredManual)> )
        return WinThread{ false, CREATE_SUSPENDED,
            x0::X0_CXX_wlfi(
                x0::PreserveArg<lambda_t>(fn),
                x0::PreserveArg<decltype(argn)>(argn) ...
            ),
            x0::X0_CXX_wafi(
                x0::PreserveArg<decltype(argn)>(argn) ...
            )
        };
}

};

#endif