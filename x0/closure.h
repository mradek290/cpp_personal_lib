
#ifndef X0_CXX_CLOSURE_H
#define X0_CXX_CLOSURE_H

#include "meta.h"

namespace x0 {

template< class ... Tn >
class closure;

template<>
class closure<>{
public:

    typedef void value_t;
    static const unsigned int rank = 0;

    template< unsigned int N >
    void get() const {
    }

    template< class lambda_t, class ... Xn >
    decltype(auto) apply( lambda_t&& fn, Xn&& ... argn ){
        return fn( PreserveArg<decltype(argn)>(argn) ... );
    }

    template< class lambda_t, class ... Xn >
    decltype(auto) apply( lambda_t&& fn, const Xn& ... argn ) const {
        return fn( PreserveArg<decltype(argn)>(argn) ... );
    }
};

template< unsigned int N >
struct X0_CLOSURE_helper{

    template< class T >
    static constexpr auto& get( T& v ){
        return X0_CLOSURE_helper<N-1>::get( static_cast< typename T::super& >(v) );
    }

    template< class T >
    static constexpr const auto& get( const T& v ){
        return X0_CLOSURE_helper<N-1>::get( static_cast< const typename T::super& >(v) );
    }
};

template<>
struct X0_CLOSURE_helper<0>{

    template< class T >
    static constexpr auto& get( T& v ){
        return v.data;
    }

    template< class T >
    static constexpr const auto& get( const T& v ){
        return v.data;
    }
};

template< class T, class ... Tn >
class closure<T,Tn...> : public closure<Tn...> {

    typedef closure<Tn...> super;

    T data;

    template< unsigned int >
    friend struct X0_CLOSURE_helper;

public:

    typedef T value_t;
    static const unsigned int rank = sizeof...(Tn)+1;

    closure() = delete;
    closure( closure& ) = default;
    closure( const closure& ) = default;
    closure( closure&& ) = default;

    closure& operator=( const closure& ) = default;
    closure& operator=( closure&& ) = default;

    template< class X, class ... Xn >
    closure( X&& v, Xn&& ... argn ) :
        super{ PreserveArg<decltype(argn)>(argn)... },
        data{ PreserveArg<X>(v) }
    {
    }

    template< unsigned int N >
    auto& get(){
        return X0_CLOSURE_helper<N>::get(*this);
    }

    template< unsigned int N >
    const auto& get() const {
        return X0_CLOSURE_helper<N>::get(*this);
    }

    template< class lambda_t, class ... Xn >
    decltype(auto) apply( lambda_t&& fn, Xn&& ... argn ){
        return static_cast<super*>(this)->apply( 
            PreserveArg<decltype(fn)>(fn),
            PreserveArg<decltype(argn)>(argn) ... , data
        );
    }

    template< class lambda_t, class ... Xn >
    decltype(auto) apply( lambda_t&& fn, const Xn& ... argn ) const {
        return static_cast<const super*>(this)->apply( 
            PreserveArg<decltype(fn)>(fn),
            PreserveArg<decltype(argn)>(argn) ... , data
        );
    }
};

template< class ... Tn >
closure( Tn&& ... ) -> closure<Tn...>;

}; //End of namespace x0

#endif