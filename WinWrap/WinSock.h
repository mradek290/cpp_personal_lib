
#ifndef __WIN_WRAP_SOCKETS_HEADER__
#define __WIN_WRAP_SOCKETS_HEADER__

/*
 *  In case of linker errors append -lwsock32 and -lWs2_32
 *  to the compiler flags after the objectfile location
*/

#include "../x0/error.h"
#include <windows.h>

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#else
static_assert( 
    _WIN32_WINNT >= 0x0501 ,
    "Win32 version ill defined. Check include order or assure _WIN32_WINNT is atleast 0x501"
);
#endif

#include <winsock2.h>
#include <winsock.h>
#include <ws2tcpip.h>

namespace WinWrap{

struct WinSockError : x0::Error {
    using x0::Error::Error;
};

class {

    WSAData core;
    int state;
    unsigned long error_code;
    bool initialized = false;
    bool success = false;

public:

    void launch(){

        if( !initialized ){

            initialized = true;
            state = WSAStartup( MAKEWORD(2,2), &core );
            success = !state;
        }

        if( state ){

            error_code = WSAGetLastError();
            WinSockError e {
                "Winsock initialization failed.",
                x0::Error::FATAL,
                error_code
            };

            throw(e);
        }
    }

    int getState() const {
        return state;
    }

    bool hasSucceeded() const {
        return success;
    }

    unsigned long getErrorCode() const {
        return error_code;
    }

    void terminate(){
        WSACleanup();
    }

} WinSockInit;

class Socket;

class AddressInfo : protected addrinfo{

    friend class Socket;

    addrinfo* psi;

public:

    enum flag_t : int {
        passive = 0x01,
        canonname = 0x02,
        numerichost = 0x04,
        all = 0x0100,
        addrconfig = 0x0400,
        v4mapped = 0x0800,
        non_authoriative = 0x04000,
        secure = 0x08000,
        return_preferred_names = 0x010000,
        FQDN = 0x00020000,
        fileserver = 0x00040000,
        no_flag = 0
    };

    enum family_t : int {
        no_family = 0,
        inet = 2,
        netbios = 17,
        inet6 = 23,
        IRDA = 26,
        BTH = 32
    };

    enum type_t : int {
        stream = 1,
        dgram = 2,
        raw = 3,
        RDM = 4,
        seqpacket = 5
    };

    enum protocol_t : int {
        tcp = 6,
        udp = 17,
        rm = 113
    };

    AddressInfo(){
        SecureZeroMemory( this, sizeof(*this) );
    }

    AddressInfo( family_t fm, type_t st, protocol_t pt, flag_t fl ) :
        AddressInfo()
    {
        ai_family = fm;
        ai_socktype = st;
        ai_protocol = pt;
        ai_flags = fl;
    }

    ~AddressInfo(){
        if(psi) freeaddrinfo(psi);
    }

    family_t getFamily() const {
        return static_cast<family_t>(ai_family);
    }

    type_t getSocketType() const {
        return static_cast<type_t>(ai_socktype);
    }

    protocol_t getProtocol() const {
        return static_cast<protocol_t>(ai_protocol);
    }

    flag_t getFlags() const {
        return static_cast<flag_t>(ai_flags);
    }

    void try_resolvePort( const char* port ){

        int state = getaddrinfo( 0, port, this, &psi );
        if( state )
            throw(
                WinSockError(
                    "Unable resolve port",
                    WinSockError::CRITICAL,
                    state
                )
            );
    }

    void try_resolveAddress( const char* ipstr, const char* portstr ){
    
        int state = getaddrinfo( ipstr, portstr, this, &psi );
        if( state )
            throw(
                WinSockError(
                    "Unable resolve Address",
                    WinSockError::CRITICAL,
                    state
                )
            );
    }

    [[nodiscard]]
    Socket try_resolveConnection();
};

class Socket {

    friend class AddressInfo;
    friend class SocketSet;

    SOCKET w_socket;

    Socket( SOCKET ws ) :
        w_socket{ws}
    {}

public:

    enum buffer_policy : int {
        peek = 0x2,
        out_of_band = 0x1,
        wait_all = 0x8,
        normal = 0
    };

    enum shutdown_policy : int {
        close_send = SD_SEND,
        close_recieve = SD_RECEIVE,
        close_both = SD_BOTH
    };

    Socket() : 
        w_socket{INVALID_SOCKET}
    {}

    Socket( const AddressInfo& v ) :
        Socket{}
    {
        w_socket = socket(
            v.psi->ai_family, 
            v.psi->ai_socktype,
            v.psi->ai_protocol
        );

        if( w_socket == INVALID_SOCKET )
            throw(
                WinSockError{
                    "Unable to initialize destination",
                    WinSockError::CRITICAL,
                    static_cast<unsigned long>(WSAGetLastError())
                }
            );
    }

    void close(){
        closesocket(w_socket);
        w_socket = INVALID_SOCKET;
    }

    void close( shutdown_policy sp ){
        shutdown(w_socket, static_cast<int>(sp) );
        closesocket(w_socket);
        w_socket = INVALID_SOCKET;
    }

    void try_bind( const AddressInfo& v ){

        auto error_code = bind(
            w_socket,
            v.psi->ai_addr,
            v.psi->ai_addrlen
        );

        if( error_code == SOCKET_ERROR )
            throw(
                WinSockError{
                    "Unabel to bind socket",
                    WinSockError::FATAL,
                    static_cast<unsigned long>(WSAGetLastError())
                }
            );
    }

    void try_listen(){

        auto error_code = listen( w_socket, SOMAXCONN );
        if( error_code == SOCKET_ERROR )
            throw(
                WinSockError{
                    "Unable to listen on connection",
                    WinSockError::CRITICAL,
                    static_cast<unsigned long>(WSAGetLastError())
                }
            );
    }

    [[nodiscard]]
    Socket try_awaitConnection(){

        Socket client = accept(w_socket, 0, 0 );
        if( client.w_socket == INVALID_SOCKET )
            throw(
                WinSockError{
                    "Unable to establisch connection",
                    WinSockError::FATAL,
                    static_cast<unsigned long>(WSAGetLastError())
                }
            );

        return client;
    }

    unsigned try_recieve( void* buffer, int buffer_sz, buffer_policy bp ){

        auto ret = recv( 
            w_socket,
            reinterpret_cast<char*>(buffer),
            buffer_sz,
            bp
        );

        if( ret == SOCKET_ERROR ){
            throw(
                WinSockError{
                    "Unable to recieve message",
                    WinSockError::CRITICAL,
                    static_cast<unsigned long>(WSAGetLastError())
                }
            );
        }

        return static_cast<unsigned>(ret);
    }

    unsigned try_recieve( void* buf, int sz ){
        return try_recieve(buf,sz,normal);
    }

    unsigned try_send( const void* buffer, int buffer_sz, buffer_policy bp ){

        auto ret = send( 
            w_socket,
            reinterpret_cast<const char*>(buffer),
            buffer_sz,
            bp
        );

        if( ret == SOCKET_ERROR )
            throw(
                WinSockError{
                    "Unable to send message",
                    WinSockError::CRITICAL,
                    static_cast<unsigned long>(WSAGetLastError())
                }
            );

        return ret;
    }

    bool isValid() const {
        return w_socket != INVALID_SOCKET;
    }

    unsigned try_send( const void* buf, int sz ){
        return try_send(buf,sz,normal);
    }

    bool operator==( const Socket& v ) const {
        return w_socket == v.w_socket;
    }

    bool operator==( SOCKET v ) const {
        return w_socket == v;
    }

    bool operator!=( const Socket& v ) const {
        return w_socket != v.w_socket;
    }

    bool operator!=( SOCKET v ) const {
        return w_socket != v;
    }

    operator SOCKET&(){
        return w_socket;
    }
};

Socket AddressInfo::try_resolveConnection(){
    
    SOCKET connection;
    addrinfo* t = psi;
    int error_code;

    while( t ){

        connection = socket( 
            t->ai_family,
            t->ai_socktype,
            t->ai_protocol
        );

        if( connection == INVALID_SOCKET ){

            freeaddrinfo(t);
            throw(
                WinSockError{
                    "Unable to resolve connection",
                    WinSockError::CRITICAL,
                    static_cast<unsigned long>(WSAGetLastError())
                }
            );
        }

        error_code = connect( connection, t->ai_addr, t->ai_addrlen );
        if( error_code == SOCKET_ERROR ){
            closesocket(connection);
            connection = INVALID_SOCKET;
        }else
            break;

        t = t->ai_next;
    }

    if( connection == INVALID_SOCKET )
        throw(
            WinSockError{
                "Unable to resolve connection",
                WinSockError::CRITICAL,
                static_cast<unsigned long>(WSAGetLastError())
            }
        );
    
    return Socket{connection};
}

class SocketSet : fd_set{
public:

    static unsigned getSetCapacity(){
        return FD_SETSIZE;
    }

    unsigned getSize() const {
        return fd_count;
    }

    bool isFull() const {
        return fd_count == FD_SETSIZE;
    }

    bool isEmpty() const {
        return fd_count == 0;
    }

    void initialize(){

        fd_count = 0;
        FillMemory(
            fd_array,
            sizeof(SOCKET) * FD_SETSIZE,
            ~0
        );
    }

    SocketSet(){
        initialize();
    }

    unsigned try_await(){

        auto result = select(
            0,
            this,
            0, 0, 0
        );

        if( result == 0 || result == SOCKET_ERROR )
            throw(
                WinSockError{
                    "Socket set failed to await connections.",
                    WinSockError::CRITICAL,
                    static_cast<unsigned long>(WSAGetLastError())
                }
            );

        return result;
    }

    bool contains( Socket sc ){
        return sc.isValid() && __WSAFDIsSet( sc, this );
    }

    bool contains( unsigned n ){
        return fd_array[n] != INVALID_SOCKET && __WSAFDIsSet( fd_array[n], this );
    }

    bool addToSet( Socket sc ){

        if( sc.isValid() && !isFull() ){
            fd_array[fd_count++] = sc;
            return true;
        }

        return false;
    }

    unsigned loadSockets( Socket* scs, unsigned scs_sz ){

        unsigned ret = 0;
        unsigned i = 0;
        while( i < scs_sz && i+fd_count < getSetCapacity() ){

            if( scs[i].isValid() ){

                fd_array[fd_count++] = scs[i];
                ++ret;
            }

            ++i;
        }

        return ret;
    }

    void validate(){

        auto new_sz = fd_count;
        for( unsigned i = 0; i < fd_count; ++i ){

            unsigned j = i;
            while( (fd_array[j] == INVALID_SOCKET || !contains(j)) && j < fd_count ){
                ++j;
                --new_sz;
            }

            fd_array[i] = fd_array[j];
            i = j;
        }

        fd_count = new_sz;
    }

    Socket removeFromSet( unsigned n ){

        if( n < fd_count ){

            Socket ret = operator[](n);
            fd_array[n] = fd_array[--fd_count];
            fd_array[fd_count+1] = INVALID_SOCKET;
            return ret;
        }

        return Socket{};
    }

    Socket& operator[]( unsigned n ){
        return reinterpret_cast<Socket*>(fd_array)[n];
    }

    const Socket& operator[]( unsigned n ) const {
        return reinterpret_cast<const Socket*>(fd_array)[n];
    }
};

}; //End of namespace WinWrap

#endif