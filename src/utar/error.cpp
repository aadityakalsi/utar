/*! error.cpp */

#include <utar/error-int.hpp>

#include <string.h>

namespace utar {

char const* error_msg(error_code c)
{
    char const* msg;
    if (c < 0) {
        msg = strerror(-c);
    } else if (c > 0) {
        switch (c) {
        case short_read:
            msg = "short read from file";
            break;
        case short_write:
            msg = "short write to file";
            break;
        default:
            msg = "unknown error";
            break;
        }
    } else { // err == 0
        msg = "";
    }
    return msg;
}

}//namespace utar
