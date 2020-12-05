#ifndef __WIN_WRAP_CONSOLE_HEADER__
#define __WIN_WRAP_CONSOLE_HEADER__

#include <windows.h>
#include "..\ips_string.h"


//////////////////////////////////////////////////////////////////////////

namespace WinWrap{

const char* linebreak = "\n";

struct WinConsole{

    static const unsigned int DEFAULT_BUFFER_SIZE = 0xFF;

    enum color{

        black = 0,
        navy = 1,
        green = 2,
        mint = 3,
        crimson = 4,
        purple = 5,
        beige = 6,
        white = 7,
        gray = 8,
        blue = 9,
        lime = 10,
        teal = 11,
        red = 12,
        pink = 13,
        yellow = 14,
        snow = 15
    };

private:

    void* ocon;
    void* icon;
    unsigned long temporary;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    CONSOLE_SCREEN_BUFFER_INFO initial_settings;

    WinConsole& operator=( const WinConsole& ) = delete;

public:

    WinConsole( void* pocon = GetStdHandle(STD_OUTPUT_HANDLE),
                void* picon = GetStdHandle(STD_INPUT_HANDLE) );

    short getDimX();
    short getDimY();
    short getCursorX();
    short getCursorY();
    void  setCursorPosition( short, short );
    void  moveCursor( short, short );
    void  resetCursor();
    void  resetCursorX();
    void  resetCursorY();

    void setTextColor( color );
    void setBackgroundColor( color );
    void resetStyle();

	inline void newline();

    unsigned int getLastInputSize() const;
    void flush();
    inline const void* operator()(const void*);

    template< class T >
    WinConsole& operator<<( T );

	template< const unsigned long base_size >
	WinConsole& operator<<( ips_string<base_size>& );

    WinConsole& operator<<( const char* );

    template< class T >
    WinConsole& operator>>( T& );

	template< const unsigned long base_size >
	WinConsole& operator>>( ips_string<base_size>& );

    WinConsole& operator>>( char* );
    char readCharacter();
    unsigned char readByte();
    unsigned write_to_buffer( char* , unsigned );

} con;

};

#ifndef __WIN_WRAP_CONSOLE_IMPLEMENTATION__
#include "WinConsole.cpp"
#endif

#endif
