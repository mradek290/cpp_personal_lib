#ifndef __WIN_WRAP_FILE_HEADER__
#define __WIN_WRAP_FILE_HEADER__

#include <Windows.h>
#include "StaticString.h"
 
//////////////////////////////////////////////////////////////////////////

namespace WinWrap{
	
using StaticString::char_t;
using FixedArray::len_t;
typedef long long filesize_t;

struct WinFile{

    static const len_t MAX_FILE_PATH_LENGTH = 1024;

    typedef StaticString::static_string<MAX_FILE_PATH_LENGTH> fname_t;

    enum AccessRight{

        FULL  = GENERIC_ALL,
        READ  = GENERIC_READ,
        WRITE = GENERIC_WRITE,
        IO    = GENERIC_READ | GENERIC_WRITE
    };

    enum ShareMode{

        EXCLUSIVE   = 0x00000000,
        SHARED      = 0x00000004,
        SHARE_WRITE = 0x00000002,
        SHARE_READ  = 0x00000001
    };

    enum OpenMode{

        OVERRIDE       = 2, /*always creates a new file*/
        ATTEMPT_CREATE = 1, /*only succeeds if the file doesn't exists*/
        NORMAL         = 4, /*opens existing file, creates new one if there is none*/
        ATTEMPT_OPEN   = 3, /*only succeeds on existing files*/
        TRUNCATED      = 5  /*only succeeds on existing files, zeros contents*/
    };

    enum Attribute{

        ARCHIVED   = 0x00000020,
        ENCRYPTED  = 0x00004000,
        HIDDEN     = 0x00000002,
        DEFAULT    = 0x00000080,
        READONLY   = 0x00000001,
        SYSTEM     = 0x00000004,
        EPHEMERAL  = 0x04000000,
        UNBUFFERED = 0x20000000,
        DIRECT     = 0x80000000
    };

    struct Flag{

        OpenMode    open_flag;
        ShareMode   share_flag;
        Attribute   attribute_flag;
        AccessRight access_flag;
    };

    static const Flag access_default;
    static const Flag access_read;
    static const Flag access_write;
    static const Flag access_create;

private:

    HANDLE   fptr;
    fname_t* fname;
    Flag     flags;

    WinFile( const WinFile& ) = delete;
    WinFile& operator=( const WinFile& ) = delete;

public:

    WinFile();
    WinFile( const char_t*, Flag );
    WinFile( const char_t*, OpenMode, ShareMode, Attribute, AccessRight );
    ~WinFile();

    bool isValid() const;
    filesize_t getSize();
    bool abortIO();

    const char_t* getFullname();
    const char_t* getFilename();
    const char_t* getExtension();

    void newline();

    len_t read( void*, len_t );
    len_t write( const void*, len_t );

	FILETIME getCreationTime();
	FILETIME getLastAccessTime();
	FILETIME getLastWriteTime();
	void getAllTimes( FILETIME*, FILETIME*, FILETIME* ); 

    filesize_t setFilePointer( filesize_t );
    filesize_t moveFilePointer( filesize_t );
    filesize_t getFilepointer();

    void close();
    bool open();
    bool open( const char_t*, Flag );
    bool open( const char_t*, OpenMode, ShareMode, Attribute, AccessRight );

    bool makeCopy( const char_t*, bool = false );
    bool erase();

    template< const len_t friend_size >
    void operator<<( StaticString::static_string<friend_size>& );

    template< const len_t friend_size >
    void operator>>( StaticString::static_string<friend_size>& );

    /*static bool MakeDirectory( const char_t* );
    static bool EraseDirectory( const char_t* );*/
};
	
};

#ifndef __WIN_WRAP_FILE_IMPLEMENTATION__
#include "WinFile.cpp"
#endif

#endif