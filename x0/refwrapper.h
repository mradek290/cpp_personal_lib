
#ifndef X0_CXX_REF_WRAPPER
#define X0_CXX_REF_WRAPPER

#include "meta.h"

namespace x0 {

class X0_REF_WRAPPER_base{

    enum value_category{
        value,
        ref,
        rvalue
    };
};

template< class T >
class ReferenceWrapper : X0_REF_WRAPPER_base {

    static_assert( sizeof(T) == 0, "Attempting to wrap l-value" );

    T element;

public:

    static const value_category category = value;

    explicit ReferenceWrapper( const T& v ) :
        element{v}
    {
    }

    T get() const {
        return element;
    }

    const T& read() const {
        return element;
    }
};

template< class T >
class ReferenceWrapper<T&> : X0_REF_WRAPPER_base {

    T& element;

public:

    static const value_category category = ref;

    ReferenceWrapper() = delete;

    ReferenceWrapper( T& v ) :
        element{v}
    {
    }

    T& get(){
        return element;
    }

    const T& read() const {
        return element;
    }
};

template< class T >
class ReferenceWrapper<T&&> : NoCopyClass, X0_REF_WRAPPER_base {

    T element;

public:

    static const value_category category = rvalue;

    ReferenceWrapper() = delete;

    ReferenceWrapper( ReferenceWrapper&& v ) :
        element{ PreserveArg<T>(v.element) }
    {
    }

    ReferenceWrapper( T&& v ) :
        element{ PreserveArg<T>(v) }
    {
    }

    T&& get() {
        return Transfer(element);
    }

    const T& read() const {
        return element;
    }
};

template< class T >
auto Wrap( T& v ){
    return ReferenceWrapper<RemoveRef<T>&>(v);
}

template< class T >
auto Wrap( T&& v ){
    return ReferenceWrapper<RemoveRef<T>&&>( PreserveArg<T>(v) );
}

}; //End of namespace x0

#endif