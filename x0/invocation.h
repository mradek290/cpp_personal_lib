
#ifndef X0_CXX_INVOCATION
#define X0_CXX_INVOCATION

#include "closure.h"
#include "refwrapper.h"

namespace x0 {

template< class lambda_t, class closure_t >
class Invocation : NoCopyClass {

    lambda_t fn;
    closure_t args;

public:

    Invocation( lambda_t&& pfn, closure_t&& pargs ) :
        fn{ PreserveArg<decltype(pfn)>(pfn) },
        args{ PreserveArg<decltype(pargs)>(pargs) }
    {
    }

    decltype(auto) invoke(){
        return args.apply(fn);
    }

    typedef decltype( FauxRef<Invocation>().invoke() ) return_t;
};

template< class lambda_t, class ... Tn >
auto X0_CXX_wlfi( lambda_t&& fn, Tn&& ... argn ){
    return  [xfn = PreserveArg<lambda_t>(fn)]
            ( ReferenceWrapper<decltype(argn)>& ... xarg ) mutable -> decltype(auto)
            { return xfn( PreserveArg<decltype(xarg.get())>(xarg.get()) ... ); };
}

template< class ... Tn >
auto X0_CXX_wafi( Tn&& ... argn ){
    return closure{ ReferenceWrapper<decltype(argn)>( PreserveArg<decltype(argn)>(argn) ) ... };
}

template< class lambda_t, class ... Tn >
auto Invoke( lambda_t&& fn, Tn&& ... argn ){
    return Invocation{
        X0_CXX_wlfi(
            PreserveArg<lambda_t>(fn),
            PreserveArg<decltype(argn)>(argn) ...
        ),
        X0_CXX_wafi(
            PreserveArg<decltype(argn)>(argn) ...
        )
    };
}

}; //End of namespace x0

#endif 