
#ifndef X0_CXX_ERROR_H
#define X0_CXX_ERROR_H

namespace x0 {

class Error{

public:

    enum error_type : unsigned long {
        DEFAULT = 0,
        NO_ERROR,
        NORMAL,
        CRITICAL,
        FATAL
    };

private:

    struct{

        union{

            unsigned long long u_eid;
            struct{

                unsigned long u_code;
                union{

                    Error::error_type e_type;
                    unsigned long u_type;
                };
            };   
        };
    } info;

    const char* message;

public:

    Error();
    Error( const char* );
    Error( const char*, error_type );
    Error( const char*, error_type, unsigned long );
    Error( error_type );
    Error( error_type, unsigned long );
    Error( const Error& ) = default;
    Error( Error&& ) = default;

    const char* getMessage() const;
    unsigned long getTypeId() const;
    error_type getType() const;
    unsigned long getCode() const;
    unsigned long long getFlag() const;
};

}; //End of namespace x0

#ifndef X0_CXX_ERROR_I
#include "error.cpp"
#endif

#endif