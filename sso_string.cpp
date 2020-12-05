
#ifndef SSO_STRING_IMPL
#define SSO_STRING_IMPL

#include "sso_string.h"

void sso_string::truncate_front( len_t n ){
    erase(0,n);
}

void sso_string::truncate_end( len_t n ){
    erase( getSize() - n, n ); 
}

bool sso_string::operator!=( const char* str ) const {

    len_t str_sz = strlen(str);
    return str_sz != getSize() ? true : memcmp( str, c_str(), str_sz );
}

bool sso_string::operator==( const char* str ) const {

    len_t str_sz = strlen(str);
    return str_sz == getSize() ? !memcmp( str, c_str(), str_sz ) : false;
}

bool sso_string::operator!=( const sso_string& v ) const {

    if( is_small() ){

        len_t sz = get_small_size();
        if( v.is_small() )
            return sz != v ?
                   true :
                   memcmp( data, v.data, sz );
        else
            return sz != v.content.stop - v.content.start ?
                   true :
                   memcmp( data, v.content.start, sz );
    }

    len_t sz = content.stop - content.start;
    if( v.is_small() )
        return sz != v.get_small_size() ?
               true :
               memcmp( content.start, v.data, sz );
    else
        return sz != v.content.stop - v.content.start ?
               true :
               memcmp( content.start, v.content.start, sz );
}

bool sso_string::operator==( const sso_string& v ) const {

    if( is_small() ){

        len_t sz = get_small_size();
        if( v.is_small() )
            return sz == v.get_small_size() ?
                   !memcmp( data, v.data, sz ) :
                   false;
        else
            return v.content.stop - v.content.start == sz ?
                   !memcmp( data, v.content.start, sz ) :
                   false;
    }

    len_t sz = content.stop - content.start;
    if( v.is_small() )
        return sz == v.get_small_size() ?
               !memcmp( content.start, v.data, sz  ) :
               false;
    else
        return sz == v.content.stop - v.content.start ?
               !memcmp( content.start, v.content.start, sz ) :
               false;
}

sso_string::operator bool() const {

    return !isEmpty();
}

sso_string::len_t sso_string::occurences( char v, len_t offset ) const {

    len_t retval = 0;

    offset = locate(v,offset);
    while( offset != INVALID_INDEX ){

        ++retval;
        offset = locate(v,offset+1);
    }

    return retval;
}

sso_string::len_t sso_string::occurences( const char* v, len_t offset ) const {

    len_t retval = 0;

    offset = locate(v,offset);
    while( offset != INVALID_INDEX ){

        ++retval;
        offset = locate(v,offset+1);
    }

    return retval;
}

sso_string::len_t sso_string::occurences( const sso_string& v, len_t offset ) const {

    len_t retval = 0;

    offset = locate(v,offset);
    while( offset != INVALID_INDEX ){

        ++retval;
        offset = locate(v,offset+1);
    }

    return retval;
}

bool sso_string::contains( char v, len_t o ) const {

    return locate(v,o) != INVALID_INDEX;
}

bool sso_string::contains( const char* v, len_t o ) const {

    return locate(v,o) != INVALID_INDEX;
}

bool sso_string::contains( const sso_string& v, len_t o ) const {

    return locate(v,o) != INVALID_INDEX;
}

bool sso_string::isFull() const {

    return is_small() ?
        !get_small_capacity() :
        block.start == content.start && block.stop == content.stop;
}

void sso_string::shrink(){

    if( is_small() || (block.start == content.start && block.stop == content.stop) )
        return;

    len_t sz = content.stop - content.start;
    char* t = new char[sz];

    memcpy( t, content.start, sz );
    delete [] block.start;

    block.start = t;
    block.stop  = t + sz;
    content.start = t;
    content.stop  = block.stop;
}

void sso_string::allocate_for_c_str( len_t sz, len_t cap ){

    block.start = new char[cap];
    block.stop  = block.start + cap;
    content.start = block.start + cap/2 - sz/2;
    content.stop  = content.start + sz;
}

void sso_string::transform_to_large( len_t cap ){

    len_t sz = get_small_size();
    char* t[] = { block.start,
                  block.stop,
                  content.start,
                  content.stop
    };

    cap = 2*(cap+sz);
    allocate_for_c_str(sz, cap < 2*INTERNAL_SIZE ? 2*INTERNAL_SIZE : cap );
    memcpy( content.start, &t[0], sz );
}

sso_string::len_t sso_string::get_small_capacity() const {

    return data[INTERNAL_SIZE];
}

sso_string::len_t sso_string::get_small_size() const {

    return INTERNAL_SIZE - data[INTERNAL_SIZE] + 1;
}

char& sso_string::operator[]( long n ) {

    return is_small() ?
        data[n] :
        content.start[n];
}

const char& sso_string::operator[]( long n ) const {

    return is_small() ?
        data[n] :
        content.start[n];
}

template< class lambda >
void sso_string::for_all( lambda lam ){

    if( is_small() ){

        char* t0 = data;
        char* t1 = t0 + get_small_size();

        while( t0 < t1 )
            lam(*t0++);

    }else{

        char* t0 = block.start;
        while( t0 < block.stop )
            lam(*t0++);
    }
}

template< class lambda >
void sso_string::for_content( lambda lam ){

    if( is_small() ){

        char* t0 = data;
        char* t1 = t0 + get_small_size();

        while( t0 < t1 )
            lam(*t0++);

    }else{

        char* t0 = content.start;
        while( t0 < content.stop )
            lam(*t0++);
    }
}

void sso_string::swap( sso_string& v ){

    char* t[] = { block.start,
                  block.stop,
                  content.start,
                  content.stop
    };

    block   = v.block;
    content = v.content;

    v.block.start   = t[0];
    v.block.stop    = t[1];
    v.content.start = t[2];
    v.content.stop  = t[3];
}

char* sso_string::getBuffer(){

    if( is_small() )
        return reinterpret_cast<char*>(&data[0]);

    return block.start;
}

const char* sso_string::c_str() const {

    if( is_small() )
        return reinterpret_cast<const char*>(&data[0]);

    return content.start;
}

bool sso_string::isEmpty() const {

    return is_small() ?
        get_small_capacity() == INTERNAL_SIZE :
        content.start == content.stop;
}

sso_string& sso_string::operator=( sso_string&& v ){

    if( &block.start == &v.block.start ) return *this;

    if( !is_small() )
        delete [] block.start;

    block = v.block;
    content = v.content;
    v.init();

    return *this;
}

sso_string::sso_string( sso_string&& v ){

    block = v.block;
    content = v.content;
    v.init();
}

sso_string::sso_string( const sso_string& v ){

    if( v.is_small() ){

        block = v.block;
        content = v.content;

    }else{

        len_t sz = v.block.stop - v.block.start;
        block.start = new char[sz];
        block.stop  = block.start + sz;
        content.start = block.start + ( v.content.start - v.block.start );
        content.stop  = block.start + ( v.content.stop  - v.block.start );
        memcpy( content.start, v.content.start, v.content.stop - v.content.start );

    }
}

sso_string::sso_string( const char* str ){

    len_t sz = strlen(str);
    if( is_valid_string(str,sz) ){

        if( sz > INTERNAL_SIZE ){

            ++sz; //include null-terminator into memory operations
            allocate_for_c_str(sz,2*sz);
            memcpy( content.start, str, sz );

        }else{

            init();
            memcpy( &data[0], str, sz );
            set_small_size(sz);
        }
    }
}

sso_string::sso_string(){

    init();
}

sso_string::~sso_string(){

    if( !is_small() )
        delete [] block.start;
}

sso_string::len_t sso_string::getFreeCapacity() const {

    return is_small() ?
        get_small_capacity() :
        (block.stop - block.start) - (content.stop - content.start);
}

sso_string::len_t sso_string::getCapacity() const {

    return is_small() ?
        INTERNAL_SIZE :
        block.stop - block.start - 1;
}

sso_string::len_t sso_string::getSize() const {

    return is_small() ?
        get_small_size()-1 :
        content.stop - content.start -1;
}

void sso_string::set_small_size( unsigned char sz ){

    data[INTERNAL_SIZE] = INTERNAL_SIZE - sz;
}

bool sso_string::is_small() const {

    return content.stop - static_cast<char*>(0) < INTERNAL_SIZE+1;
}

void sso_string::init(){

    block.start = 0;
    block.stop = 0;
    content.start = 0;
    content.stop = 0;
    set_small_size(0);
}

void sso_string::flush(){

    if( is_small() ){
        init();
    }else{
        content.start = block.start + (block.stop-block.start)/2;
        content.stop = content.start +1;
        content.start[0] = 0;
        content.start[-1] = 0;
    }
}

sso_string& sso_string::operator=( const char* str ){

    len_t sz = strlen(str);
    if( is_valid_string(str,sz) ){

        if( sz > getCapacity() ){

            flush();
            ++sz; //include null-terminator into memory operations
            allocate_for_c_str(sz,2*sz);
            memcpy( content.start, str, sz );

        }else{

            if( is_small() ){

                content.stop = 0;
                memcpy( &data[0], str, sz );
                data[sz] = 0;
                set_small_size(sz);

            }else{

                ++sz;
                content.start = block.start + ( block.stop - block.start )/2 - sz/2;
                content.stop  = content.start + sz;
                memcpy( content.start, str, sz );
            }
        }

    }else{

        flush();
    }

    return *this;
}

sso_string& sso_string::operator=( const sso_string& v ){

    if( this == &v ) return *this;

    if( is_small() ){

        if( v.is_small() ){

            block = v.block;
            content = v.content;

        }else{

            len_t sz = v.content.stop - v.content.start;
            allocate_for_c_str(sz,2*sz);
            memcpy( content.start, v.content.start, sz );
        }

    }else{

        if( v.is_small() ){

            len_t sz = v.get_small_size();
            content.start = block.start + ( block.stop - block.start )/2 - sz/2;
            content.stop  = content.start + sz;
            memcpy( content.start, &v.data[0], sz );

        }else{

            len_t sz  = v.content.stop - v.content.start;
            len_t cap = block.stop - block.start;

            if( sz > cap ){

                delete [] block.start;
                block.start = new char[2*sz];
                block.stop  = block.start + 2*sz;
            }

            content.start = block.start + ( block.stop - block.start )/2 - sz/2;
            content.stop  = content.start + sz;
            memcpy( content.start, v.content.start, sz );
        }

    }

    return *this;
}

void sso_string::pad( char c, len_t amnt ){

    if( is_small() ){

        transform_to_large(amnt);

    }else{

        len_t delta = content.start - block.start;
        if( amnt > delta )
            reallocate(amnt);
    }

    content.start -= amnt;
    memset( content.start, c, amnt );
}

void sso_string::p_pad( const char* str, len_t sz ){

    if( is_small() ){

        transform_to_large(sz);

    }else{

        len_t delta = content.start - block.start;
        if( sz > delta )
            reallocate(sz);
    }

    content.start -= sz;
    memmove( content.start, str, sz );
}

void sso_string::pad( const char* str ){

    len_t sz = strlen(str);
    if( is_valid_string(str,sz) )
        p_pad(str,sz);
}

void sso_string::pad( const sso_string& v ){

    if( isEmpty() ) return;
    p_pad( v.c_str(), v.getSize() );
}

sso_string::sso_string( char c, len_t n ){

    ++n; // account for null-terminator
    if( n > INTERNAL_SIZE ){

        allocate_for_c_str(n,n);
        memset( content.start, c, n-1 );
        *(content.stop-1) = 0;

    }else{

        init();
        memset( data, c, n-1 );
        set_small_size(n-1);
    }
}

void sso_string::reallocate( len_t ncap ){

    ncap = (3*(ncap + (block.stop - block.start)))/2;
    len_t sz  = content.stop - content.start;
    char* nb0 = new char[ncap];
    char* nc0 = nb0 + ncap/2 - sz/2;

    memcpy( nc0, content.start, sz );
    delete [] block.start;

    block.start = nb0;
    block.stop  = nb0 + ncap;
    content.start = nc0;
    content.stop  = nc0 + sz;
}

void sso_string::append( char c, len_t amnt ){

    if( is_small() ){

        if( amnt > get_small_capacity() ){

            transform_to_large(amnt);
            memset( content.stop-1, c, amnt );
            content.stop += amnt;
            *(content.stop-1) = 0;

        }else{

            len_t sz = get_small_size()-1;
            memset( data + sz, c, amnt );
            set_small_size(sz+amnt);
        }

    }else{

        len_t delta = block.stop - content.stop;
        if( amnt > delta )
            reallocate(amnt);

        memset( content.stop-1, c, amnt );
        content.stop += amnt;
        *(content.stop-1) = 0;
    }
}

void sso_string::p_append( const char* str, len_t sz ){

    if( is_small() ){

        if( sz > get_small_capacity() ){

            transform_to_large(sz);
            memcpy( content.stop-1, str, sz );
            content.stop += sz;
            *(content.stop-1) = 0;

        }else{

            len_t smallsize = get_small_size()-1;
            memcpy( data + smallsize, str, sz );
            set_small_size( smallsize+sz );
        }

    }else{

        len_t delta = block.stop - content.stop;
        if( sz > delta )
            reallocate(sz);

        memmove( content.stop-1, str, sz );
        content.stop += sz;
        *(content.stop-1) = 0;
    }
}

void sso_string::append( const char* str ){

    len_t sz = strlen(str);
    if( is_valid_string(str,sz) )
        p_append(str,sz);
}

void sso_string::append( const sso_string& v ){

    if( isEmpty() ) return;
    p_append( v.c_str(), v.getSize() );
}

sso_string::len_t sso_string::locate( char c, len_t offset ) const {

    if( is_small() ){

        len_t sz = get_small_size();
        if( offset > sz ) return INVALID_INDEX;

        char* t = static_cast<char*>( memchr( data + offset, c, sz - offset ) );
        return t ? t - data : INVALID_INDEX;
    }

    len_t sz = content.stop - content.start;
    if( offset > sz ) return INVALID_INDEX;

    char* t = static_cast<char*>( memchr( content.start + offset, c, sz - offset ) );
    return t ? t - content.start : INVALID_INDEX;
}

sso_string::len_t sso_string::locate( const char* str, len_t offset ) const {

    if( is_small() ){

        if( offset > get_small_size() ) return INVALID_INDEX;
        char* t = strstr( data + offset, str );
        return t ? t - data : INVALID_INDEX;
    }

    if( offset > content.stop - content.start ) return INVALID_INDEX;
    char* t = strstr( content.start + offset, str );
    return t ? t - content.start : INVALID_INDEX;
}

sso_string::len_t sso_string::locate( const sso_string& v, len_t offset ) const {

    if( is_small() ){

        if( v.is_small() ){

            if( offset + v.get_small_size() > get_small_size() ) return INVALID_INDEX;
            char* t = strstr( data + offset, v.data );
            return t ? t - data : INVALID_INDEX;

        }else{

            return INVALID_INDEX;
        }

    }else{

        if( v.is_small() ){

            if( offset + v.get_small_size() > content.stop - content.start ) return INVALID_INDEX;
            char* t = strstr( content.start, v.data );
            return t ? t - content.start : INVALID_INDEX;

        }else{

            if( offset + (v.content.stop - v.content.start) > content.stop - content.start )
                return INVALID_INDEX;

            char* t = strstr( content.start, v.content.start );
            return t ? t - content.start : INVALID_INDEX;
        }
    }
}

sso_string::len_t sso_string::locate_reverse( char c, len_t offset ) const {

    len_t str_sz = 1;
    char str[] = {c,0};

    if( is_small() ){

        len_t this_sz = get_small_size();
        if( offset + str_sz > this_sz ) return INVALID_INDEX;
        return r_find( data, this_sz - offset, str, str_sz );

    }else{

        len_t this_sz = content.stop - content.start;
        if( offset + str_sz > this_sz ) return INVALID_INDEX;
        return r_find( content.start, this_sz-offset, str, str_sz );
    }
}

sso_string::len_t sso_string::locate_reverse( const char* str, len_t offset ) const {

    len_t str_sz = strlen(str);
    if( is_valid_string(str,str_sz) ){

        if( is_small() ){

            len_t this_sz = get_small_size();
            if( offset + str_sz > this_sz ) return INVALID_INDEX;
            return r_find( data, this_sz - offset, str, str_sz );

        }else{

            len_t this_sz = content.stop - content.start;
            if( offset + str_sz > this_sz ) return INVALID_INDEX;
            return r_find( content.start, this_sz-offset, str, str_sz );
        }
    }

    return INVALID_INDEX;
}

sso_string::len_t sso_string::locate_reverse( const sso_string& v, len_t offset ) const {

    return locate_reverse( v.c_str(), offset );
}

bool sso_string::substring( sso_string& v, len_t pos, len_t len ) const {

    return subrange(v, pos, pos+len );
}

bool sso_string::subrange( sso_string& v, len_t p0, len_t p1 ) const {

    if( p1 < p0 ){

        len_t t = p1;
        p1 = p0;
        p0 = t;
    }
    len_t range = p1 - p0;

    if( is_small() ){

        len_t this_sz = get_small_size();
        if( &v == this || p1 > this_sz ) return false;

        if( v.is_small() ){ // this and v are small

            v.init();
            memcpy( v.data, data + p0, range );
            v.set_small_size(range);

        }else{ // this is small. v is not

            v.content.start = v.block.start + ( v.block.stop - v.block.start )/2 - range/2;
            v.content.stop  = v.content.start + range + 1; // account for null-terminator
            memcpy( v.content.start, data + p0, range );
            *(v.content.stop-1) = 0;
        }

    }else{

        len_t this_sz = content.stop - content.start;
        if( &v == this || p1 > this_sz ) return false;

        if( v.is_small() ){ // this is large. v is small

            if( range < INTERNAL_SIZE ){ // edge case: range fits into a small string

                v.init();
                memcpy( v.data, content.start + p0, range );
                v.set_small_size(range);

            }else{ // range does not fit into a small string

                v.allocate_for_c_str( range+1, 2*range );
                memcpy( v.content.start, content.start + p0, range );
                *(v.content.stop-1) = 0;
            }

        }else{ // this and v are both large

            ++range; // account for null-terminator
            if( range > v.block.stop - v.block.start ){

                delete [] v.block.start;
                v.block.start = new char[2*range];
                v.block.stop  = v.block.start + 2*range;
            }

            v.content.start = v.block.start + ( v.block.stop - v.block.start )/2 - range/2;
            v.content.stop  = v.content.start + range;
            memcpy( v.content.start, content.start + p0, range-1 );
            *(v.content.stop-1) = 0;
        }
    }

    return true;
}

void sso_string::erase( len_t pos, len_t amnt ){

    if( is_small() ){

        len_t sz = get_small_size();
        if( pos > sz ) return;

        amnt = (amnt > sz-pos-1) ? sz-pos-1 : amnt;
        memmove( data + pos, data + pos + amnt, sz - pos - amnt );
        memset( data + sz - amnt, 0, INTERNAL_SIZE - sz + amnt );
        set_small_size( sz-amnt-1 );

    }else{

        len_t sz = content.stop - content.start;
        if( pos > sz ) return;

        amnt = (amnt > sz-pos-1) ? sz-pos-1 : amnt;
        memmove( content.start + pos, content.start + pos + amnt, sz - pos - amnt );
        content.stop -= amnt;
    }
}

void sso_string::insert( char c, len_t pos, len_t cpy ){

    if( is_small() ){

        len_t sz = get_small_size();
        if( pos > sz ) return;
        if( sz + cpy > INTERNAL_SIZE || pos + cpy > INTERNAL_SIZE ){

            transform_to_large(cpy);

        }else{

            memmove( data+pos+cpy, data+pos, sz-pos );
            memset( data+pos, c, cpy );
            set_small_size( sz + cpy -1 );

            return;
        }
    }

    len_t sz  = content.stop - content.start;
    if( pos > sz ) return;

    if( cpy > block.stop - content.stop )
        reallocate(cpy);

    memmove( content.start+pos+cpy, content.start+pos, sz-pos );
    memset( content.start+pos, c, cpy );
    content.stop += cpy;
}

void sso_string::p_insert( const char* str, len_t str_sz, len_t pos ){

    if( is_small() ){

        len_t this_sz = get_small_size();
        if( pos > this_sz ) return;
        if( this_sz + str_sz > INTERNAL_SIZE || pos + str_sz > INTERNAL_SIZE ){

            transform_to_large(str_sz);

        }else{

            memmove( data+pos+str_sz, data+pos, this_sz-pos );
            memcpy( data+pos, str, str_sz );
            set_small_size( this_sz + str_sz -1 );

            return;
        }
    }

    len_t this_sz = content.stop - content.start;
    if( pos > this_sz ) return;

    if( str_sz > block.stop - content.stop )
        reallocate(str_sz);

    memmove( content.start+pos+str_sz, content.start+pos, this_sz-pos );
    memcpy( content.start+pos, str, str_sz );
    content.stop += str_sz;
}

void sso_string::insert( const char* str, len_t pos ){

    len_t sz = strlen(str);
    if( is_valid_string(str,sz) )
        p_insert(str,sz,pos);
}

void sso_string::insert( const sso_string& v, len_t pos ){

    if( !isEmpty() )
        p_insert( c_str(), getSize(), pos );
}

void sso_string::emplace( char c, len_t pos, len_t cpy ){

    if( is_small() ){

        len_t sz = get_small_size();
        if( pos > sz ) return;

        if( pos+cpy > INTERNAL_SIZE ){

           transform_to_large(cpy);

        }else{

            memset( data+pos, c, cpy );
            if( pos+cpy >= sz )
                set_small_size( pos+cpy );

            return;
        }
    }

    len_t sz = content.stop - content.start;
    if( pos > sz ) return;

    if( content.start + pos + cpy + 1 > block.stop )
        reallocate(cpy);

    memset( content.start + pos, c, cpy );

    char* emplace_end = content.start + pos + cpy+1; //account for null-terminator
    if( emplace_end > content.stop ){

        content.stop = emplace_end;
        *(emplace_end-1) = 0;
    }
}

void sso_string::p_emplace( const char* str, len_t str_sz, len_t pos ){

    if( is_small() ){

        len_t this_sz = get_small_size();
        if( pos > this_sz ) return;

        if( pos+str_sz > INTERNAL_SIZE ){

            transform_to_large(str_sz);

        }else{

            memcpy( data+pos, str, str_sz );
            if( pos+str_sz >= this_sz )
                set_small_size( pos+str_sz );

            return;
        }
    }

    len_t this_sz = content.stop - content.start;
    if( pos > this_sz ) return;

    if( content.start + pos + str_sz + 1 > block.stop )
        reallocate(str_sz);

    memmove( content.start + pos, str, str_sz );

    char* emplace_end = content.start + pos + str_sz + 1;
    if( emplace_end > content.stop ){

        content.stop = emplace_end;
        *(emplace_end-1) = 0;
    }
}

void sso_string::emplace( const char* str, len_t pos ){

    len_t sz = strlen(str);
    if( is_valid_string(str,sz) )
        p_emplace(str,sz,pos);
}

void sso_string::emplace( const sso_string& v, len_t pos ){

    if( !isEmpty() )
        p_emplace( c_str(), getSize(), pos );
}

char& sso_string::getFirst(){

    if( is_small() )
        return data[0];
    else
        return *content.start;
}

char& sso_string::getLast(){

    if( is_small() )
        return data[get_small_size()-2];
    else
        return *(content.stop-2);
}

const char& sso_string::getFirst() const {

    if( is_small() )
        return data[0];
    else
        return *content.start;
}

const char& sso_string::getLast() const {

    if( is_small() )
        return data[get_small_size()-2];
    else
        return *(content.stop-2);
}

sso_string sso_string::operator+( const sso_string& v ) const {

    if( is_small() ){

        len_t this_sz = get_small_size()-1;
        if( v.is_small() ){

            len_t v_sz = v.get_small_size()-1;
            len_t joint_size = this_sz + v_sz;

            if( joint_size > INTERNAL_SIZE ){

                sso_string retval( 0, joint_size );
                memcpy( retval.block.start, data, this_sz );
                memcpy( retval.block.start + this_sz, v.data, v_sz );
                *(retval.block.stop-1) = 0;

                return retval;

            }else{

                sso_string retval( 0, joint_size );
                memcpy( retval.data, data, this_sz );
                memcpy( retval.data + this_sz, v.data, v_sz );
                retval.set_small_size(joint_size);

                return retval;
            }

        }else{

            len_t v_sz = v.content.stop - v.content.start;
            len_t joint_size = this_sz + v_sz;
            sso_string retval( 0, joint_size);

            memcpy( retval.block.start, data, this_sz );
            memcpy( retval.block.start + this_sz, v.content.start, v_sz+1 );

            return retval;
        }

    }else{

        len_t this_sz = content.stop - content.start;
        if( v.is_small() ){

            len_t v_sz = v.get_small_size()-1;
            len_t joint_size = this_sz + v_sz;
            sso_string retval( 0, joint_size );

            memcpy( retval.block.start, content.start, this_sz );
            memcpy( retval.block.start + this_sz-1, v.data, v_sz );
            *(retval.block.stop-1) = 0;

            return retval;

        }else{

            len_t v_sz = v.content.stop - v.content.start-1;
            len_t joint_size = this_sz + v_sz;
            sso_string retval( 0, joint_size );

            memcpy( retval.block.start, content.start, this_sz );
            memcpy( retval.block.start + this_sz, v.content.start, v_sz+1 );

            return retval;
        }
    }

    return sso_string{}; // inaccessable
}

sso_string sso_string::operator+( const char* str ) const {

    len_t str_sz = strlen(str);
    if( is_valid_string(str,str_sz) ){

        if( is_small() ){

            len_t this_sz = get_small_size()-1;
            len_t joint_sz = this_sz + str_sz;

            if( joint_sz > INTERNAL_SIZE ){

                sso_string retval( 0, joint_sz );
                memcpy( retval.block.start, data, this_sz );
                memcpy( retval.block.start + this_sz, str, str_sz+1 );

                return retval;

            }else{

                sso_string retval( 0, joint_sz );
                memcpy( retval.data, data, this_sz );
                memcpy( retval.data + this_sz, str, str_sz );
                retval.set_small_size(joint_sz);

                return retval;
            }

        }else{

            len_t this_sz = content.stop - content.start -1;
            len_t joint_sz = str_sz + this_sz;
            sso_string retval( 0, joint_sz );

            memcpy( retval.block.start, content.start, this_sz );
            memcpy( retval.block.start + this_sz, str, str_sz+1 );

            return retval;
        }
    }

    return sso_string{*this};
}

#endif
