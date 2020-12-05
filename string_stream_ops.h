
#ifndef CRTP_STRING_STREAM_OPS_HEADER
#define CRTP_STRING_STREAM_OPS_HEADER

struct stream_ops_settings{

    typedef unsigned int len_t;

    static const  len_t INVALID_INDEX = len_t(-1);
    static const  len_t BUFFER_SIZE = 64;
    inline static len_t radix = 10;
    inline static len_t long precision = 16;

    static bool is_valid_string( const char* str, len_t sz ){

        return str && sz && *str;
    }

    static len_t r_find( const char* str, len_t str_size, const char* sub, len_t sub_size ){

        if( sub_size < str_size ){

            const char* rstr = str+str_size-1;
            const char* rsub = sub+sub_size-1;
            const char* t0;
            const char* t1;

            while( rstr > str ){

                t0 = rstr;
                t1 = rsub;

                while( *t0 == *t1 && t0 > str && t1 > sub ){
                    --t0;
                    --t1;
                }

                if( t1 == sub && *t1 == *t0 )
                    return t0-str;
                else
                    --rstr;
            }

        }

        return INVALID_INDEX;
    }

    template< const len_t N >
    constexpr len_t STR_LEN_DEDUCTION( char const (&str)[N] ){

        return N;
    }

};

template< class base_string >
class stringstream_ops : public stream_ops_settings {

    base_string* get_base(){

        return reinterpret_cast<base_string*>(this); // i know i know
    }

public:

    /** Following member-function signatures must be provided by
        base_string:

        len_t getSize()
        const char* c_str()
        void append( const char* )
        void append( const base_string )
        base_string& operator=( const base_string& )
    */

    void operator+=( char c ){

        get_base()->append(c);
    }

    void operator+=( const char* str ){

        get_base()->append(str);
    }

    void operator+=( const base_string& v ){

        get_base()->append(v);
    }


//////////////////////////////////////////////////////////////////
/// L-stream ops
//////////////////////////////////////////////////////////////////

    base_string& operator<<( bool b ){

        base_string* t = get_base();
        char buffer[BUFFER_SIZE];

        if( b )
            t->append("True");
        else
            t->append("False");

        return *t;
    }

    base_string& operator<<( const void* v ){

        base_string* t = get_base();
        char buffer[BUFFER_SIZE];

        buffer[0] = '0';
        buffer[1] = 'x';

        if constexpr( sizeof(v) == 8 )
            _ui64toa_s( reinterpret_cast<unsigned long long>(v), buffer+2, BUFFER_SIZE-2, 16 );
        else
            _ultoa_s( reinterpret_cast<unsigned int>(v), buffer+2, BUFFER_SIZE, 16 );

        t->append( buffer );

        return *t;
    }

    base_string& operator<<( const base_string& v ){

        base_string* t = get_base();
        t->append(v);
        return *t;
    }

    base_string& operator<<( const char* v ){

        base_string* t = get_base();
        t->append(v);
        return *t;
    }

    base_string& operator<<( double f ){

        static_assert( BUFFER_SIZE >= 2, "Converting floating point types requires atleast a 2 byte buffer. Check stream_ops_settings::BUFFER_SIZE.");

        base_string* t = get_base();
        char buffer[BUFFER_SIZE];

        _gcvt_s( buffer, BUFFER_SIZE, f, precision );
        t->append(buffer);

        return *t;
    }

    base_string& operator<<( unsigned char i ){

        base_string* t = get_base();
        char buffer[BUFFER_SIZE];

        _ultoa_s( i, buffer, BUFFER_SIZE, radix );
        t->append(buffer);

        return *t;
    }

    base_string& operator<<( unsigned short i ){

        base_string* t = get_base();
        char buffer[BUFFER_SIZE];

        _ultoa_s( i, buffer, BUFFER_SIZE, radix );
        t->append(buffer);

        return *t;
    }

    base_string& operator<<( unsigned int i ){

        base_string* t = get_base();
        char buffer[BUFFER_SIZE];

        _ultoa_s( i, buffer, BUFFER_SIZE, radix );
        t->append(buffer);

        return *t;
    }

    base_string& operator<<( unsigned long i ){

        base_string* t = get_base();
        char buffer[BUFFER_SIZE];

        _ultoa_s( i, buffer, BUFFER_SIZE, radix );
        t->append(buffer);

        return *t;
    }

    base_string& operator<<( unsigned long long i ){

        base_string* t = get_base();
        char buffer[BUFFER_SIZE];

        _ui64toa_s( i, buffer, BUFFER_SIZE, radix );
        t->append(buffer);

        return *t;
    }

    base_string& operator<<( char c ){

        base_string* t = get_base();
        t->append(c);
        return *t;
    }

    base_string& operator<<( short i ){

        base_string* t = get_base();
        char buffer[BUFFER_SIZE];

        _itoa_s( i, buffer, BUFFER_SIZE, radix );
        t->append(buffer);

        return *t;
    }

    base_string& operator<<( int i ){

        base_string* t = get_base();
        char buffer[BUFFER_SIZE];

        _itoa_s( i, buffer, BUFFER_SIZE, radix );
        t->append(buffer);

        return *t;
    }

    base_string& operator<<( long i ){

        base_string* t = get_base();
        char buffer[BUFFER_SIZE];

        _ltoa_s( i, buffer, BUFFER_SIZE, radix );
        t->append(buffer);

        return *t;
    }

    base_string& operator<<( long long i ){

        base_string* t = get_base();
        char buffer[BUFFER_SIZE];

        _i64toa_s( i, buffer, BUFFER_SIZE, radix );
        t->append(buffer);

        return *t;
    }

//////////////////////////////////////////////////////////////////
/// R-stream ops
//////////////////////////////////////////////////////////////////

    base_string& operator>>( char* str ){

        base_string* t = get_base();
        memcpy( str, t->c_str(), t->getSize() );
        return *t;
    }

    base_string& operator>>( base_string& v ){

        base_string* t = get_base();
        t->operator=(v);
        return *t;
    }

    base_string& operator>>( float& f ){

        base_string* t = get_base();
        f = atof(t->c_str());
        return *t;
    }

    base_string& operator>>( double& f ){

        base_string* t = get_base();
        f = strtod(t->c_str(),0);
        return *t;
    }

    base_string& operator>>( unsigned char& i ){

        base_string* t = get_base();
        i = static_cast<unsigned char>( strtoul( t->c_str(), 0, radix ) );
        return *t;
    }

    base_string& operator>>( unsigned short& i ){

        base_string* t = get_base();
        i = static_cast<unsigned short>( strtoul( t->c_str(), 0, radix ) );
        return *t;
    }

    base_string& operator>>( unsigned int& i ){

        base_string* t = get_base();
        i = static_cast<unsigned int>( strtoul( t->c_str(), 0, radix ) );
        return *t;
    }

    base_string& operator>>( unsigned long& i ){

        base_string* t = get_base();
        i = static_cast<unsigned long>( strtoul( t->c_str(), 0, radix ) );
        return *t;
    }

    base_string& operator>>( unsigned long long& i ){

        base_string* t = get_base();
        i = static_cast<unsigned long long>( strtoull(t->c_str(), 0, radix ) );
        return *t;
    }

    base_string& operator>>( char& c ){

        base_string* t = get_base();
        c = t->c_str()[0];
        return *t;
    }

    base_string& operator>>( short& i ){

        base_string* t = get_base();
        i = static_cast<short>( strtol( t->c_str(), 0, radix ) );
        return *t;
    }

    base_string& operator>>( int& i ){

        base_string* t = get_base();
        i = static_cast<int>( strtol( t->c_str(), 0, radix ) );
        return *t;
    }

    base_string& operator>>( long& i ){

        base_string* t = get_base();
        i = static_cast<long>( strtol( t->c_str(), 0, radix ) );
        return *t;
    }

    base_string& operator>>( long long& i ){

        base_string* t = get_base();
        i = static_cast<long long>( strtoll(t->c_str(), 0, radix ) );
        return *t;
    }
};

#endif
