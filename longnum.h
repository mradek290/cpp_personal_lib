
#ifndef LONG_NUM_HEADER
#define LONG_NUM_HEADER

#include "stdlib.h"
#include "string.h"

class LongNumCore{
public:

    struct {} inline static Uninitialized;
    typedef signed index_t;
    typedef long long number_t;
    typedef char byte_t;
    static const unsigned long long UnsignedMax = ~0;
    static const long long SignedMax = 0x7fffffffffffffff;
};

template<
    LongNumCore::index_t alph_sz,
    LongNumCore::byte_t (*toByte)( LongNumCore::byte_t ),
    LongNumCore::byte_t (*toAlph)( LongNumCore::byte_t ),
    bool (*isAlph)( LongNumCore::byte_t )
> class LongNumFormatInfo{
public:

    static constexpr LongNumCore::index_t GetAlphabetSize();
    static LongNumCore::byte_t AlphabetToByte( LongNumCore::byte_t );
    static LongNumCore::byte_t ByteToAlphabet( LongNumCore::byte_t );
    static bool IsAlphabet( LongNumCore::byte_t );
};

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
class UnsignedLongNum : LongNumCore {

    bool valid;
    index_t cnt;
    storage_t dat[digits];

    static number_t b_search( const UnsignedLongNum&, const UnsignedLongNum&, UnsignedLongNum&, number_t, number_t );

    void clear_leading_zeros();

public:

    using LongNumCore::Uninitialized;

    static constexpr number_t getBase();
    static constexpr index_t  getCapacity();
    index_t getMagnitude() const;
    index_t getSerializationBufferSize() const;

    void zero();

    UnsignedLongNum();
    UnsignedLongNum( decltype(Uninitialized) );
    UnsignedLongNum( const UnsignedLongNum& ) = default;
    UnsignedLongNum( UnsignedLongNum&& ) = default;
    UnsignedLongNum( storage_t );

    UnsignedLongNum& operator=( const UnsignedLongNum& ) = default;
    UnsignedLongNum& operator=( UnsignedLongNum&& ) = default;
    UnsignedLongNum& operator=( storage_t );
    template< class format >
    void assign( const byte_t* );

    storage_t toInt() const;
    double    toDouble() const;

    storage_t& operator[]( index_t );
    const storage_t& operator[]( index_t ) const;

    index_t serialize( void* ) const;
    index_t deserialize( const void* );

    void shl( index_t );
    void shr( index_t );
    void placement_shl( const UnsignedLongNum&, index_t );
    void placement_shr( const UnsignedLongNum&, index_t );

    void placement_add( const UnsignedLongNum&, const UnsignedLongNum& );
    void placement_sub( const UnsignedLongNum&, const UnsignedLongNum& );
    void placement_mul( const UnsignedLongNum&, const UnsignedLongNum& );
    void placement_div( const UnsignedLongNum&, const UnsignedLongNum& );
    void placement_mod( const UnsignedLongNum&, const UnsignedLongNum& );

    void placement_add( const UnsignedLongNum&, storage_t );
    void placement_sub( const UnsignedLongNum&, storage_t );
    void placement_mul( const UnsignedLongNum&, storage_t );
    void placement_div( const UnsignedLongNum&, storage_t );
    void placement_mod( const UnsignedLongNum&, storage_t );

    void add( const UnsignedLongNum& );
    void sub( const UnsignedLongNum& );

    void add( storage_t );
    void sub( storage_t );
    void mul( storage_t );
    void div( storage_t );
    void mod( storage_t );

    bool isZero() const;

    bool operator==( const UnsignedLongNum& ) const;
    bool operator!=( const UnsignedLongNum& ) const;
    bool operator<=( const UnsignedLongNum& ) const;
    bool operator>=( const UnsignedLongNum& ) const;
    bool operator< ( const UnsignedLongNum& ) const;
    bool operator> ( const UnsignedLongNum& ) const;

    bool operator==( storage_t ) const;
    bool operator!=( storage_t ) const;
    bool operator<=( storage_t ) const;
    bool operator>=( storage_t ) const;
    bool operator< ( storage_t ) const;
    bool operator> ( storage_t ) const;
};

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits>
class SignedLongNum : private UnsignedLongNum<storage_t,base,digits>{

    typedef UnsignedLongNum<storage_t,base,digits> parent_t;

    bool positive;

public:

    using LongNumCore::Uninitialized;

    using parent_t::getBase;
    using parent_t::getCapacity;
    using parent_t::getMagnitude;
    
    SignedLongNum();
    SignedLongNum( decltype(Uninitialized) );
    SignedLongNum( const SignedLongNum& ) = default;
    SignedLongNum( SignedLongNum&& ) = default;
    SignedLongNum( storage_t );

    LongNumCore::index_t getSerializationBufferSize() const;
    void zero();
    void negate();
    void abs();
    void flip();

    SignedLongNum& operator=( const SignedLongNum& ) = default;
    SignedLongNum& operator=( SignedLongNum&& ) = default;
    SignedLongNum& operator=( storage_t );

    using parent_t::assign;

    storage_t toInt() const;
    double    toDouble() const;
    const UnsignedLongNum<storage_t,base,digits>& toUnsigned() const;

    using parent_t::operator[];

    LongNumCore::index_t serialize( void* ) const; //TODO 
    LongNumCore::index_t deserialize( const void* ); //TODO 

    using parent_t::shl;
    using parent_t::shr;
    using parent_t::isZero;

    void placement_shl( const SignedLongNum&, LongNumCore::index_t );
    void placement_shr( const SignedLongNum&, LongNumCore::index_t );

    void placement_add( const SignedLongNum&, const SignedLongNum& );
    void placement_sub( const SignedLongNum&, const SignedLongNum& );
    void placement_mul( const SignedLongNum&, const SignedLongNum& );
    void placement_div( const SignedLongNum&, const SignedLongNum& );
    void placement_mod( const SignedLongNum&, const SignedLongNum& );

    /*
    void placement_add( const SignedLongNum&, storage_t );
    void placement_sub( const SignedLongNum&, storage_t );
    void placement_mul( const SignedLongNum&, storage_t );
    void placement_div( const SignedLongNum&, storage_t );
    void placement_mod( const SignedLongNum&, storage_t );
    */

    /*
    void add( storage_t );
    void sub( storage_t );
    void mul( storage_t );
    void div( storage_t );
    void mod( storage_t );
    */

    bool isPositive() const;
    bool isNegative() const;

    bool operator==( const SignedLongNum& ) const;
    bool operator!=( const SignedLongNum& ) const;
    bool operator<=( const SignedLongNum& ) const;
    bool operator>=( const SignedLongNum& ) const;
    bool operator< ( const SignedLongNum& ) const;
    bool operator> ( const SignedLongNum& ) const;

    bool operator==( storage_t ) const;
    bool operator!=( storage_t ) const;
    bool operator<=( storage_t ) const;
    bool operator>=( storage_t ) const;
    bool operator< ( storage_t ) const;
    bool operator> ( storage_t ) const;
};

#ifndef LONG_NUM_F_IMPL
#include "flongnum.cpp"
#endif

#ifndef LONG_NUM_U_IMPL
#include "ulongnum.cpp"
#endif

#ifndef LONG_NUM_S_IMPL
#include "slongnum.cpp"
#endif

#endif
