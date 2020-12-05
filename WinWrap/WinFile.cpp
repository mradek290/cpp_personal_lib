#ifndef __WIN_WRAP_FILE_IMPLEMENTATION__
#define __WIN_WRAP_FILE_IMPLEMENTATION__

#ifndef __WIN_WRAP_FILE_HEADER__
#include "WinFile.h"
#endif

namespace WinWrap{

FILETIME WinFile::getCreationTime(){
	
	FILETIME ft;
	GetFileTime( fptr, &ft, 0, 0 );
	return ft;
}

FILETIME WinFile::getLastAccessTime(){
	
	FILETIME ft;
	GetFileTime( fptr, 0, &ft, 0 );
	return ft;
}

FILETIME WinFile::getLastWriteTime(){
	
	FILETIME ft;
	GetFileTime( fptr, 0, 0, &ft );
	return ft;
}

void WinFile::getAllTimes( FILETIME* ct, FILETIME* at, FILETIME* wt ){
	
	GetFileTime( fptr, ct, at, wt );
}

const WinFile::Flag WinFile::access_default = {

    WinFile::OpenMode::NORMAL,
    WinFile::ShareMode::EXCLUSIVE,
    WinFile::Attribute::DEFAULT,
    WinFile::AccessRight::IO
};

const WinFile::Flag WinFile::access_read = {

    WinFile::OpenMode::ATTEMPT_OPEN,
    WinFile::ShareMode::SHARE_READ,
    WinFile::Attribute::DEFAULT,
    WinFile::AccessRight::READ
};

const WinFile::Flag WinFile::access_write = WinFile::access_default;

const WinFile::Flag WinFile::access_create = {

    WinFile::OpenMode::OVERRIDE,
    WinFile::ShareMode::EXCLUSIVE,
    WinFile::Attribute::DEFAULT,
    WinFile::AccessRight::FULL
};

filesize_t WinFile::moveFilePointer( filesize_t offset ){

    LARGE_INTEGER retval{0};
    LARGE_INTEGER lios;

    lios.QuadPart = offset;
    SetFilePointerEx( fptr, lios, &retval, FILE_CURRENT );
    return retval.QuadPart;
}

template< const len_t friend_size >
void WinFile::operator>>( StaticString::static_string<friend_size>& v ){

    LARGE_INTEGER dummy;
    LARGE_INTEGER offset;

    ReadFile( fptr, v.getBuffer(), v.getCapacity(), &(dummy.u.LowPart), 0 );
    v.validate();

    offset.QuadPart = static_cast<filesize_t>(v.getSize()) - static_cast<filesize_t>(dummy.u.LowPart);
    SetFilePointerEx( fptr, offset, &dummy, FILE_CURRENT );
}

const char_t* WinFile::getFullname(){

    return fname->getBuffer();
}

const char_t* WinFile::getFilename(){

    len_t pos = fname->locate_reverse("\\");
    if( pos < fname->getCapacity() ){

        return fname->getBuffer() + pos + 1;
    }

    return "";
}

const char_t* WinFile::getExtension(){

    len_t pos = fname->locate_reverse(".");
    if( pos < fname->getCapacity() ){

        return fname->getBuffer() + pos;
    }

    return "";
}

template< const len_t friend_size >
void WinFile::operator<<( StaticString::static_string<friend_size>& v ){

    write( v.getBuffer(), v.getSize() );
}

bool WinFile::erase(){

    close();
    return DeleteFile(fname->getBuffer());
}

/*bool WinFile::EraseDirectory( const char_t* str ){

    return RemoveDirectoryA(str);
}

bool WinFile::MakeDirectory( const char_t* str ){

    return CreateDirectory(str,0);
}*/

bool WinFile::makeCopy( const char_t* str, bool force_overwrite ){

    return CopyFile( fname->getBuffer(), str, !force_overwrite );
}

void WinFile::newline(){

    DWORD t;
    WriteFile( fptr, "\r\n", 2, &t, 0 );
}

filesize_t WinFile::getFilepointer(){

    LARGE_INTEGER dummy{0};
    LARGE_INTEGER retval{0};

    SetFilePointerEx( fptr, dummy, &retval, FILE_CURRENT );
    return retval.QuadPart;
}

filesize_t WinFile::setFilePointer( filesize_t pos ){

    LARGE_INTEGER tmp;
    LARGE_INTEGER retval;

    tmp.QuadPart = pos;
    SetFilePointerEx( fptr, tmp, &retval, FILE_BEGIN );
    return retval.QuadPart;
}

len_t WinFile::read( void* buf, len_t sz ){

    len_t retval = 0;
    ReadFile( fptr, buf, sz, &retval, 0 );
    return retval;
}

len_t WinFile::write( const void* buf, len_t sz ){

    len_t retval = 0;
    WriteFile( fptr, buf, sz, &retval, 0 );
    return retval;
}

filesize_t WinFile::getSize(){

    LARGE_INTEGER retval{0};
    GetFileSizeEx( fptr, &retval );
    return retval.QuadPart;
}

bool WinFile::abortIO(){

    return CancelIo(fptr);
}

WinFile::WinFile() :
    fptr(0),
    fname( new fname_t ),
    flags{ WinFile::OpenMode::ATTEMPT_OPEN,
           WinFile::ShareMode::SHARED,
           WinFile::Attribute::DEFAULT,
           WinFile::AccessRight::IO }{
}

WinFile::WinFile( const char_t* str, Flag f ) :
    fptr(0),
    fname( new fname_t(str) ),
    flags(f){

    open();
}

WinFile::WinFile( const char_t* str, OpenMode om, ShareMode sm, Attribute atb, AccessRight ar ) :
    fptr(0),
    fname( new fname_t(str) ),
    flags{ om, sm, atb, ar }{

    open();
}

WinFile::~WinFile(){

    close();
    delete fname;
}

bool WinFile::open(){

    close();
    fptr = CreateFile( fname->getBuffer(), flags.access_flag, flags.share_flag, 0, flags.open_flag, flags.attribute_flag, 0 );
    return isValid();
}

bool WinFile::open( const char_t* str, Flag f ){

    close();

    GetFullPathNameA( str, fname->getFreeCapacity(), fname->getBuffer(), 0 );
    fname->validate();

    flags = f;
    fptr = CreateFile( str, flags.access_flag, flags.share_flag, 0, flags.open_flag, flags.attribute_flag, 0 );
    return isValid();
}

bool WinFile::open( const char_t* str, OpenMode om, ShareMode sm, Attribute atb, AccessRight ar ){

    close();

    GetFullPathNameA( str, fname->getFreeCapacity(), fname->getBuffer(), 0 );
    fname->validate();

    flags.access_flag = ar;
    flags.open_flag = om;
    flags.share_flag = sm;
    flags.attribute_flag = atb;
    fptr = CreateFile( str, ar, sm, 0, om, atb, 0 );
    return isValid();
}

bool WinFile::isValid() const {

    return fptr && fptr != INVALID_HANDLE_VALUE;
}

void WinFile::close(){

    if( isValid() ){

        CloseHandle(fptr);
        fptr = 0;
    }
}
	
};

#endif