
#ifndef X0_CXX_UNSTORAGE
#define X0_CXX_UNSTORAGE

#include <new>
#include "meta.h"

namespace x0 {

union {} DefaultInitializer;

template< class T >
class unstorage : NoCopyClass{

    static_assert(
        !IsSameType< decltype(DefaultInitializer), T >, 
        "DefaultInitializer may only be used on explicit template instantiation."
    );

    alignas(alignof(T)) unsigned char dummy__[sizeof(T)];

public:

    constexpr unstorage() = default;

    constexpr unstorage( decltype(DefaultInitializer) ){
        new(this) T{};
    }

    constexpr unstorage( const T& v ){
        new(this) T{v};
    }

    constexpr unstorage( T&& v ){
        new(this) T{PreserveArg<decltype(v)>(v)};
    }

    template< class ... Tn >
    constexpr unstorage( Tn&& ... argn ){
        new(this) T{ PreserveArg<decltype(argn)>(argn) ... };
    }

    T& object(){
        return *reinterpret_cast<T*>(this);
    }

    const T& object() const {
        return *reinterpret_cast<const T*>(this);
    }

    unstorage& operator=( const T& v ){
        object() = v;
        return *this;
    }

    unstorage& operator=( T&& v ){
        object() = PreserveArg<decltype(v)>(v);
        return *this;
    }

    template< class ... Tn >
    void init( Tn&& ... argn ){
        new(this) T{ PreserveArg<decltype(argn)>(argn) ... };
    }

    void init( decltype(DefaultInitializer) ){
        new(this) T{};
    }

    void kill(){
        object().~T();
    }

    operator T&(){
        return object();
    }

    operator const T&() const {
        return object();
    }
};

}; // End of namespace x0

#endif