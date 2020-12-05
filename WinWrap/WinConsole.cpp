#ifndef __WIN_WRAP_CONSOLE_IMPLEMENTATION__
#define __WIN_WRAP_CONSOLE_IMPLEMENTATION__

#ifndef __WIN_WRAP_CONSOLE_HEADER__
#include "WinConsole.h"
#endif

namespace WinWrap{

void WinConsole::setTextColor( color col ){


    GetConsoleScreenBufferInfo( ocon, &csbi );
    auto bg = csbi.wAttributes / 0x10;
    SetConsoleTextAttribute( ocon, (bg*0x10) | col );
}

void WinConsole::setBackgroundColor( color col ){

    GetConsoleScreenBufferInfo( ocon, &csbi );
    auto bg = csbi.wAttributes & 0xFFFF00FF;
    SetConsoleTextAttribute( ocon, static_cast<int>(col)*0x10 | bg );
}

void WinConsole::resetStyle(){

    SetConsoleTextAttribute( ocon, initial_settings.wAttributes );
}

void WinConsole::resetCursor(){

    SetConsoleCursorPosition( ocon, {0,0} );
}

void WinConsole::resetCursorX(){

    GetConsoleScreenBufferInfo( ocon, &csbi );
    SetConsoleCursorPosition( ocon, { 0, csbi.dwCursorPosition.Y } );
}

void WinConsole::resetCursorY(){

    GetConsoleScreenBufferInfo( ocon, &csbi );
    SetConsoleCursorPosition( ocon, { csbi.dwCursorPosition.X, 0 } );
}

void WinConsole::moveCursor( short dx, short dy ){

    GetConsoleScreenBufferInfo( ocon, &csbi );
    COORD pos = csbi.dwCursorPosition;

    if( dx > pos.X )
        pos.X = 0;
    else
        pos.X += dx;

    if( dy > pos.Y )
        pos.Y = 0;
    else
        pos.Y += dy;

    if( pos.X > csbi.dwSize.X )
        pos.X = csbi.dwSize.X;

    if( pos.Y > csbi.dwSize.Y )
        pos.Y = csbi.dwSize.Y;

    SetConsoleCursorPosition( ocon, pos );
}

void WinConsole::setCursorPosition( short x, short y ){

    SetConsoleCursorPosition( ocon, {x,y} );
}

short WinConsole::getCursorX(){

    GetConsoleScreenBufferInfo( ocon, &csbi );
    return csbi.dwCursorPosition.X;
}

short WinConsole::getCursorY(){

    GetConsoleScreenBufferInfo( ocon, &csbi );
    return csbi.dwCursorPosition.Y;
}

short WinConsole::getDimX(){

    GetConsoleScreenBufferInfo( ocon, &csbi );
    return csbi.dwSize.X;
}

short WinConsole::getDimY(){

    GetConsoleScreenBufferInfo( ocon, &csbi );
    return csbi.dwSize.Y;
}

unsigned int WinConsole::getLastInputSize() const {

    return temporary;
}

void WinConsole::flush(){

    COORD zz = {0,0};
    GetConsoleScreenBufferInfo( ocon, &csbi );
    SetConsoleTextAttribute( ocon, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
    SetConsoleCursorPosition( ocon, zz );

    FillConsoleOutputAttribute( 
        ocon,
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
        csbi.dwSize.X * csbi.dwSize.Y,
        zz,
        &temporary
    );

    FillConsoleOutputCharacter( 
        ocon,
        ' ',
        csbi.dwSize.X * csbi.dwSize.Y,
        zz,
        &temporary
    );
}

WinConsole::WinConsole( void* po, void* pi ) :
    ocon(po), icon(pi) {
    GetConsoleScreenBufferInfo( ocon, &initial_settings );
}

inline const void* WinConsole::operator()( const void* v ){

    return v;
}

char WinConsole::readCharacter(){

    char buf[DEFAULT_BUFFER_SIZE];
    ReadConsole( icon, buf, DEFAULT_BUFFER_SIZE, &temporary, 0 );
    return buf[0];
}

unsigned char WinConsole::readByte(){

    unsigned char buf[DEFAULT_BUFFER_SIZE];
    ReadConsole( icon, buf, DEFAULT_BUFFER_SIZE, &temporary, 0 );
    return buf[0];
}

inline void WinConsole::newline(){

	*this << linebreak;
}

WinConsole& WinConsole::operator>>( char* v ){

    ReadConsole( icon, v, DEFAULT_BUFFER_SIZE, &temporary, 0 );
    return *this;
}

unsigned WinConsole::write_to_buffer( char* buf, unsigned sz ){

    ReadConsole( icon, buf, sz, &temporary, 0 );
    temporary -= 2;
    buf[temporary] = 0;
    return temporary;
}

/* template< unsigned long base_size >
WinConsole& WinConsole::operator>>( ips_string<base_size>& str ){

    char buffer[base_size];
    ReadConsole( icon, buffer, base_size, &temporary, 0 );

    str.append(buffer);
    return *this;
} */

template< class T >
WinConsole& WinConsole::operator>>( T& v ){

    ips_string<DEFAULT_BUFFER_SIZE> str;
    ReadConsole( icon, str.getBuffer(), str.getFreeCapacity(), &temporary, 0 );

    str.validate();
    str >> v;

    return *this;
}

template< const unsigned long base_size >
WinConsole& WinConsole::operator<<( ips_string<base_size>& str ){

    WriteConsole( ocon, str.c_str(), str.getSize(), &temporary, 0 );
    return *this;
}

WinConsole& WinConsole::operator<<( const char* s ){
    WriteConsole( ocon, s, strlen(s), &temporary, 0);
    return *this;
}

template< class T >
WinConsole& WinConsole::operator<< ( T v ){

	ips_string<DEFAULT_BUFFER_SIZE> str;
	str << v;
	WriteConsole( ocon, str.c_str(), str.getSize(), &temporary, 0 );

	return *this;
}

};

#endif
