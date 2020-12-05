
#ifndef X0_CXX_META_H
#define X0_CXX_META_H

namespace x0{

template< class T >
T& FauxRef();

//----------------------------------------------------------

class NoCopyClass{
protected:
    NoCopyClass() = default;
    NoCopyClass( const NoCopyClass& ) = delete;
    NoCopyClass& operator=( const NoCopyClass& ) = delete;
};

class NoMoveClass{
protected:
    NoMoveClass() = default;
    NoMoveClass( NoMoveClass&& ) = delete;
    NoMoveClass& operator=( NoMoveClass&& ) = delete;
};

class LockedClass : NoCopyClass, NoMoveClass{
protected:
    using NoCopyClass::NoCopyClass;
    using NoCopyClass::operator=;
    using NoMoveClass::NoMoveClass;
    using NoMoveClass::operator=;
};

//---------------------------------------------------------

class Verify{
public:
    static constexpr bool check(){
        return true;
    }
};

class Falsify{
public:
    static constexpr bool check(){
        return false;
    }
};

template< class T, class U >
class X0_META_type_compare_helper : public Falsify{};

template< class T >
class X0_META_type_compare_helper<T,T> : public Verify{};

template< class T, class U >
constexpr bool IsSameType = X0_META_type_compare_helper<T,U>::check();

//---------------------------------------------------------

template< class T, class U >
struct X0_META_implicit_conv_helper{

    static bool dummy(U);
    static int  dummy(...);
    static constexpr bool value = sizeof(decltype(dummy(FauxRef<T>()))) == 1;
};

template< class T, class U >
constexpr bool HasImplicitConversion = X0_META_implicit_conv_helper<T,U>::value;

//---------------------------------------------------------

template< class T0, class ... Tn >
struct X0_META_uniform_param_pack_helper;

template< class T0 >
struct X0_META_uniform_param_pack_helper<T0> {
    static constexpr bool value = true;
};

template< class T0, class T1, class ... Tn >
struct X0_META_uniform_param_pack_helper<T0,T1,Tn...> {
    static constexpr bool value = IsSameType<T0,T1> && X0_META_uniform_param_pack_helper<T0,Tn...>::value;
};

template< class T0, class ... Tn >
constexpr bool IsUniformParamPack = X0_META_uniform_param_pack_helper<T0,Tn...>::value;

//---------------------------------------------------------

template< class T0, class ... Tn >
struct X0_META_conv_param_pack_helper;

template< class T0 >
struct X0_META_conv_param_pack_helper<T0> {
    static constexpr bool value = true;
};

template< class T0, class T1, class ... Tn >
struct X0_META_conv_param_pack_helper<T0,T1,Tn...> {
    static constexpr bool value = HasImplicitConversion<T1,T0> && X0_META_conv_param_pack_helper<T0,Tn...>::value;
};

template< class T0, class ... Tn >
constexpr bool IsCompatibleParamPack = X0_META_conv_param_pack_helper<T0,Tn...>::value;

//---------------------------------------------------------

template< unsigned N, class ... Tn >
struct X0_META_param_pack_selector;

template< unsigned N, class T0, class ... Tn >
struct X0_META_param_pack_selector<N,T0,Tn...> : public X0_META_param_pack_selector<N-1,Tn...> {
    typedef typename X0_META_param_pack_selector<N-1,Tn...>::type type;
};

template< class T0, class ... Tn >
struct X0_META_param_pack_selector<0,T0,Tn...>{
    typedef T0 type;
};

template< unsigned N, class ... Tn >
using SelectFromParameterPack = typename X0_META_param_pack_selector<N,Tn...>::type;

//---------------------------------------------------------

/* template< class T>
class has_member{

    //! Define memberfunction signature
    template< class U, int (U::*)() > struct predicate{};

    //! Define memberfunction name
    template< class U > static constexpr char  dummy( predicate<U, &U::foo>* );

    //! SFINAEs out
    template< class U > static constexpr short dummy( ... );
public:
    static constexpr bool check(){ return sizeof( dummy<T>(0) ) == sizeof(char); }
}; */

template< class T>
class X0_META_assignemnt_op_helper{
    template< class U, T& (U::*)( const T& ) > struct predicate{};
    template< class U > static constexpr char  dummy( predicate<U, &U::operator= >* );
    template< class U > static constexpr short dummy( ... );
public:
    static constexpr bool check(){ return sizeof( dummy<T>(0) ) == sizeof(char); }
};

template< class T >
constexpr bool HasAssignmentOp = X0_META_assignemnt_op_helper<T>::check();

//-------------------------------------------------------

template< class T>
class X0_META_move_op_helper{
    template< class U, T& (U::*)( T&& ) > struct predicate{};
    template< class U > static constexpr char  dummy( predicate<U, &U::operator= >* );
    template< class U > static constexpr short dummy( ... );
public:
    static constexpr bool check(){ return sizeof( dummy<T>(0) ) == sizeof(char); }
};

template< class T >
constexpr bool HasMoveAssignOp = X0_META_move_op_helper<T>::check();

//-------------------------------------------------------

template< class T >
struct X0_META_unptr_helper{
    typedef T type;
};

template< class T >
struct X0_META_unptr_helper<T*>{
    typedef typename X0_META_unptr_helper<T>::type type;
};

template< class T >
struct X0_META_unptr_helper<T*const>{
    typedef typename X0_META_unptr_helper<T>::type type;
};

template< class T >
using RemovePtr = typename X0_META_unptr_helper<T>::type;

//-------------------------------------------------------

template< class T >
struct X0_META_unref_helper{
    typedef T type;
};

template< class T >
struct X0_META_unref_helper<T&>{
    typedef T type;
};

template< class T >
struct X0_META_unref_helper<T&&>{
    typedef T type;
};

template< class T >
using RemoveRef = typename X0_META_unref_helper<T>::type;

//-------------------------------------------------------

template< class T >
struct X0_META_unconst_helper{
    typedef T type;
};

template< class T >
struct X0_META_unconst_helper<const T>{
    typedef T type;
};

template< class T >
using RemoveConst = typename X0_META_unconst_helper<T>::type;

//--------------------------------------------------------

template< class T >
using RemoveCRef = RemoveConst< RemoveRef< T > >;

//--------------------------------------------------------

template< class T >
constexpr T&& PreserveArg( RemoveRef<T>&& arg ){
    return static_cast<T&&>(arg);
}

template< class T >
constexpr T&& PreserveArg( RemoveRef<T>& arg ){
    return static_cast<T&&>(arg);
}

template< class T >
constexpr RemoveRef<T>&& Transfer( T&& v ){
    return static_cast< RemoveRef<T>&& >(v);
}

//-------------------------------------------------------
//-------------------------------------------------------

namespace base_type_nfo{

    struct core{};
    struct scalar      : public core{};
    struct pointer     : public scalar{};
    struct number      : public scalar{};
    struct real        : public number{};
    struct integer     : public number{};
    struct x0_signed   : public integer{};
    struct x0_unsigned : public integer{};

    template< class T >
    class core_tag;

    template< class T >
    class core_tag<T*> : public pointer{};

    template<>
    class core_tag<void*> : public pointer{};

    template<>
    class core_tag<float> : public real{};

    template<>
    class core_tag<double> : public real{};

    template<>
    class core_tag<long double> : public real{};

    template<>
    class core_tag<unsigned char> : public x0_unsigned{};

    template<>
    class core_tag<unsigned short> : public x0_unsigned{};

    template<>
    class core_tag<unsigned int> : public x0_unsigned{};

    template<>
    class core_tag<unsigned long> : public x0_unsigned{};

    template<>
    class core_tag<unsigned long long> : public x0_unsigned{};

    template<>
    class core_tag<signed char> : public x0_signed{};

    template<>
    class core_tag<signed short> : public x0_signed{};

    template<>
    class core_tag<signed int> : public x0_signed{};

    template<>
    class core_tag<signed long> : public x0_signed{};

    template<>
    class core_tag<signed long long> : public x0_signed{};

    template<>
    class core_tag<char> : public integer{};

    template<>
    class core_tag<bool> : public core{};

    template<>
    class core_tag<void> : public core{};
};

template< class T >
constexpr bool IsCoreType = HasImplicitConversion<base_type_nfo::core_tag<RemoveCRef<T>>, base_type_nfo::core >;

template< class T >
constexpr bool IsInteger = HasImplicitConversion<base_type_nfo::core_tag<RemoveCRef<T>>, base_type_nfo::integer >;

template< class T >
constexpr bool IsNumber = HasImplicitConversion<base_type_nfo::core_tag<RemoveCRef<T>>, base_type_nfo::number >;

template< class T >
constexpr bool IsPointer = HasImplicitConversion<base_type_nfo::core_tag<RemoveCRef<T>>, base_type_nfo::pointer >;

template< class T >
constexpr bool IsReal = HasImplicitConversion<base_type_nfo::core_tag<RemoveCRef<T>>, base_type_nfo::real >;

template< class T >
constexpr bool IsScalar = HasImplicitConversion<base_type_nfo::core_tag<RemoveCRef<T>>, base_type_nfo::scalar >;

template< class T >
constexpr bool IsSigned = HasImplicitConversion<base_type_nfo::core_tag<RemoveCRef<T>>, base_type_nfo::x0_signed >;

template< class T >
constexpr bool IsUnsigned = HasImplicitConversion<base_type_nfo::core_tag<RemoveCRef<T>>, base_type_nfo::x0_unsigned >;

}; //End of namespace x0

#endif