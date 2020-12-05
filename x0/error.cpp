
#ifndef X0_CXX_ERROR_I
#define X0_CXX_ERROR_I

#include "error.h"

namespace x0 {

Error::Error() : message(0){
    info.u_eid = 0;
}

Error::Error( const char* msg ) : message(msg){
    info.u_eid = 0;
}

Error::Error( const char* msg, error_type et ) : message(msg){
    info.u_eid = static_cast<decltype(info.u_eid)>(et);
}

Error::Error( const char* msg, error_type et, unsigned long cd ) : message(msg){
    info.e_type = et;
    info.u_code = cd;
}

const char* Error::getMessage() const {
    return message;
}

Error::error_type Error::getType() const {
    return info.e_type;
}

unsigned long Error::getCode() const {
    return info.u_code;
}

unsigned long long Error::getFlag() const {
    return info.u_eid;
}

unsigned long Error::getTypeId() const {
    return info.u_type;
}

Error::Error( error_type et ) : message(0){
    info.u_eid = static_cast<decltype(info.u_eid)>(et);
}

Error::Error( error_type et, unsigned long cd ) : message(0){
    info.e_type = et;
    info.u_code = cd;
}

}; //End of namespace x0

#endif