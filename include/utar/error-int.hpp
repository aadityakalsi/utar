/*! error-int.hpp */

#include <utar/error.hpp>

#include <errno.h>

namespace utar {

enum err_code_name
{
    short_read = 1,
    short_write
};

inline error_code errno_error()
{
    return -errno;
}

}//namespace utar
