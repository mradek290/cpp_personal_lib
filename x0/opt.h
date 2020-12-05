
#ifndef X0_CXX_OPT_H
#define X0_CXX_OPT_H

#include "error.h"
#include "meta.h"
#include <new>

namespace x0 {

struct OptError : public Error{
    using Error::Error;
};

class X0_OPT_base{
public:

    typedef unsigned char byte;
    enum state_t : byte {
        UNINITIALIZED,
        READY
    };

};

template< class T >
class Opt : public X0_OPT_base {

    state_t state;
    alignas(alignof(T)) byte data[sizeof(T)];

    void check_integrity() const {

        if( state == UNINITIALIZED )
            throw( 
                OptError("Attemptingt to read uninitialized memory.",
                Error::CRITICAL)
            );
    }

    void destroy_value(){

        reinterpret_cast<T*>(data)->~T();
    }

    void assign( const T& v ){

        if( state == READY ){

            if constexpr( HasAssignmentOp<T> ){
                getValueReckless() = v;
            }else{
                destroy_value();
                new(data) T{v};
            }

        }else{

            new(data) T{v};
            state = READY;
        }
    }

public:

    typedef T storage_t;

    Opt() :
        state{UNINITIALIZED}
    {
    }

    Opt( const Opt& v ) :
        state{v.state}
    {
        if( v.hasValue() )
            new(data) T{v.peekValueReckless()};
    }

    Opt( const T& v ) :
        state{READY}
    {
        new(data) T{v};
    }

    Opt( T&& v ) : 
        state{READY}
    {
        new(data) T{PreserveArg<T>(v)};
    }

    ~Opt(){

        if( hasValue() )
            destroy_value();
    }

    Opt& operator=( const Opt& v ){

        if( v.hasValue() )
            assign(v.peekValueReckless());
        else
            flush();

        return *this;
    }

    Opt& operator=( const T& v ){

        assign(v);
        return *this;
    }

    Opt& operator=( T&& v ){

        if( hasValue() ){

            if constexpr( HasMoveAssignOp<T> ){

                getValueReckless() = PreserveArg<T>(v);

            }else{

                destroy_value();
                new(data) T{PreserveArg<T>(v)};
                state = READY;
            }

        }else{

            new(data) T{PreserveArg<T>(v)};
            state = READY;
        }

        return *this;
    }

    T& operator*(){
        return getValue();
    }

    T* operator->(){

        check_integrity();
        return reinterpret_cast<T*>(data);
    }

    T& getValue(){
        check_integrity();
        return getValueReckless();
    }

    const T& peekValue() const {
        check_integrity();
        return peekValueReckless();
    }

    T& getValueReckless() noexcept {
        return *reinterpret_cast<T*>(data);
    }

    const T& peekValueReckless() const noexcept {
        return *reinterpret_cast<const T*>(data);
    }

    state_t getState() const{
        return state;
    }

    bool hasValue() const {
        return state == READY;
    }

    void flush(){

    if( hasValue() ){

        destroy_value();
        state = UNINITIALIZED;
    }
}
};

//-----------------------------------------------------------

template< class T >
class Opt<T&> : public X0_OPT_base {

    T* data;

    void check_integrity() const {

        if( data ) return;
        throw( 
            OptError("Attemptingt to read uninitialized memory.",
            Error::CRITICAL)
        );
    }

public:

    typedef T storage_t;

    Opt() :
        data{0}
    {
    }

    Opt( T& v ) :
        data{&v}
    {
    }

    Opt( const T& ) = delete;
    Opt( const Opt& v ) = default;

    Opt& operator=( T& v ){
        data = &v;
        return *this;
    }

    Opt& operator=( const T& ) = delete;
    Opt& operator=( const Opt& v ) = default;

    T& operator*(){
        return getValue();
    }

    T* operator->(){
        check_integrity();
        return data;
    }

    T& getValue(){
        check_integrity();
        return getValueReckless();
    }

    const T& peekValue() const {
        check_integrity();
        return peekValueReckless();
    }

    T& getValueReckless() noexcept {
        return *data;
    }

    const T& peekValueReckless() const noexcept {
        return *data;
    }

    state_t getState() const {
        return data ? READY : UNINITIALIZED;
    }

    bool hasValue() const {
        return data;
    }

    void flush(){
        data = 0;
    }
};

//-----------------------------------------------------------

template< class T >
class Opt<const T&> : public X0_OPT_base {

    const T* data;

    void check_integrity() const {

        if( data ) return;
        throw( 
            OptError("Attemptingt to read uninitialized memory.",
            Error::CRITICAL)
        );
    }

public:

    typedef const T storage_t;

    Opt() : 
        data{0}
    {
    }

    Opt( T& ) = delete;
    Opt( const Opt& ) = default;
    Opt( const T& v ) :
        data{&v}
    {
    }

    Opt& operator=( const Opt& ) = default;
    Opt& operator=( T& ) = delete;
    Opt& operator=( const T& v ){
        data = &v;
        return *this;
    }

    const T& peekValueReckless() const noexcept {
        return *data;
    }

    const T& getValueReckless() const noexcept {
        return peekValueReckless();
    }

    const T& peekValue() const {
        check_integrity();
        return peekValueReckless();
    }

    const T& getValue() const {
        check_integrity();
        return getValueReckless();
    }

    const T& operator*() const {
        return getValue();
    }

    const T* operator->() const {
        check_integrity();
        return data;
    }

    state_t getState() const {
        return data ? READY : UNINITIALIZED;
    }

    bool hasValue() const {
        return data;
    }

    void flush(){
        data = 0;
    }
};

//-----------------------------------------------------------

template<>
class Opt<void> : public X0_OPT_base {

    void generate_error() const{

        throw(
            OptError(  "Attempting to form reference to void.", Error::CRITICAL )
        );
    }

public:

    void* operator*(){
        generate_error();
        return static_cast<void*>(0);
    }

    void* operator->(){
        generate_error();
        return static_cast<void*>(0);
    }

    void getValue(){
        generate_error();
    }

    void peekValue() const{
        generate_error();
    }

    void getValueReckless(){
        generate_error();
    }

    void peekValueReckless() const{
        generate_error();
    }

    state_t getState() const{
        return UNINITIALIZED;
    }

    bool hasValue() const{
        return false;
    }

    void flush(){}
};

}; //End of namespace x0

#endif