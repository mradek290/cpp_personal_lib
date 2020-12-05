#ifndef __WIN_RNG_HEADER__
#define __WIN_RNG_HEADER__

#include <windows.h>
#include <wincrypt.h>

namespace WinWrap{

class WinRNG{

    private:

        struct _data{

            HCRYPTPROV hcp;
            bool success;
            unsigned long refcount;
        };

		_data* dat;

        void init();
        void kill();

    public:

        WinRNG(){ init(); }
        WinRNG( const WinRNG& );
        ~WinRNG(){ kill(); }

        WinRNG& operator=( const WinRNG& );

        bool isValid() const;
        unsigned long generate32b() const;
        unsigned long long generate64b() const;
        double generate64f() const;
        void generate_sequence( void*, unsigned long ) const;
};

};

#ifndef __WIN_RNG_IMPLEMENTATION__
#include "WinRNG.cpp"
#endif

#endif
