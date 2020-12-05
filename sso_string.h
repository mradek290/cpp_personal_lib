
#ifndef SSO_STRING_HEADER
#define SSO_STRING_HEADER

#include <stdlib.h>
#include <string.h>
#include "string_stream_ops.h"

class sso_string : public stringstream_ops<sso_string>{

    static constexpr len_t INTERNAL_SIZE = 3*sizeof(void*);

    union{

        struct{

            struct{

                char* start;
                char* stop;
            } block, content;
        };

        char data[INTERNAL_SIZE];
    };

    len_t get_small_size() const;
    len_t get_small_capacity() const;
    void  set_small_size( unsigned char );

    void transform_to_large( len_t );
    void allocate_for_c_str( len_t, len_t );
    void reallocate( len_t );

    void p_append ( const char*, len_t );
    void p_insert ( const char*, len_t, len_t );
    void p_emplace( const char*, len_t, len_t );
    void p_pad( const char*, len_t );

    bool is_small() const;
    void init();

public:

    sso_string();
    sso_string( char, len_t );
    sso_string( const char* );
    sso_string( const sso_string& );
    sso_string( sso_string&& );
    ~sso_string();

    sso_string& operator=( const char* );
    sso_string& operator=( const sso_string& );
    sso_string& operator=( sso_string&& );

    sso_string  operator+( const sso_string& ) const;
    sso_string  operator+( const char* ) const;

    char& operator[]( long );
    const char& operator[]( long ) const;
    operator bool() const;

    bool isEmpty() const;
    bool isFull() const;
    len_t getSize() const;
    len_t getCapacity() const;
    len_t getFreeCapacity() const;
    const char* c_str() const;
    char* getBuffer();

    char& getFirst();
    const char& getFirst() const;
    char& getLast();
    const char& getLast() const;

    void pad( char, len_t = 1 );
    void pad( const char* );
    void pad( const sso_string& );

    void append( char, len_t = 1 );
    void append( const char* );
    void append( const sso_string& );

    len_t occurences( char, len_t = 0 ) const;
    len_t occurences( const char*, len_t = 0 ) const;
    len_t occurences( const sso_string&, len_t = 0 ) const;

    bool contains( char , len_t = 0 ) const;
    bool contains( const char*, len_t = 0 ) const;
    bool contains( const sso_string&, len_t = 0 ) const;

    len_t locate( char, len_t = 0 ) const;
    len_t locate( const char*, len_t = 0 ) const;
    len_t locate( const sso_string&, len_t = 0 ) const;

    len_t locate_reverse( char, len_t = 0 ) const;
    len_t locate_reverse( const char*, len_t = 0 ) const;
    len_t locate_reverse( const sso_string&, len_t = 0 ) const;

    bool substring( sso_string&, len_t, len_t ) const;
    bool subrange ( sso_string&, len_t, len_t ) const;

    void insert( char, len_t, len_t = 1 );
    void insert( const char*, len_t );
    void insert( const sso_string&, len_t );

    void emplace( char, len_t, len_t = 1 );
    void emplace( const char*, len_t );
    void emplace( const sso_string&, len_t );

    void erase( len_t, len_t = 1 );
    void truncate_front( len_t );
    void truncate_end( len_t );

    void shrink();
    void flush();
    void swap( sso_string& );

    bool operator==( const char* ) const;
    bool operator==( const sso_string& ) const;
    bool operator!=( const char* ) const;
    bool operator!=( const sso_string& ) const;

    template< class lambda >
    void for_all( lambda );

    template< class lambda >
    void for_content( lambda );
};

#ifndef SSO_STRING_IMPL
#include "sso_string.cpp"
#endif

#endif
