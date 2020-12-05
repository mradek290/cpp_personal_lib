#ifndef __WIN_RNG_IMPLEMENTATION__
#define __WIN_RNG_IMPLEMENTATION__

#ifndef __WIN_RNG_HEADER__
#include "WinRNG.h"
#endif

namespace WinWrap{

void WinRNG::generate_sequence( void* p, unsigned long len ) const {

    CryptGenRandom( dat->hcp, len, ((BYTE*)(p)) );
}

unsigned long WinRNG::generate32b() const {

    unsigned long val;
    CryptGenRandom( dat->hcp, sizeof(val), ((BYTE*)(&val)) );
    return val;
}

double WinRNG::generate64f() const {

    unsigned long val;
    CryptGenRandom( dat->hcp, sizeof(val), ((BYTE*)(&val)) );
    return (1.0*( val % 0xFFFFFFFF ))/0xFFFFFFFF;
}

unsigned long long WinRNG::generate64b() const {

    unsigned long long val;
    CryptGenRandom( dat->hcp, sizeof(val), ((BYTE*)(&val)) );
    return val;
}

void WinRNG::init(){

    dat = new WinRNG::_data;
    dat->success = CryptAcquireContextA( &(dat->hcp), 0, 0, PROV_RSA_AES, 0 );
    dat->refcount = 1;
}

void WinRNG::kill(){

    if( isValid() ){

        dat->refcount -= 1;
        if( dat->refcount == 0 ){

            CryptReleaseContext( dat->hcp, 0 );
            delete dat;
        }
    }

    dat = 0;
}

WinRNG::WinRNG( const WinRNG& v ){

    if( v.isValid() ){

        dat = v.dat;
        dat->refcount += 1;
    }else
        init();
}

WinRNG& WinRNG::operator=( const WinRNG& v ){

    if( dat != v.dat ){

        kill();
        dat = v.dat;
        dat->refcount += 1;
    }

    return *this;
}

bool WinRNG::isValid() const {

    return dat && dat->success && dat->refcount > 0;
}

};

#endif
