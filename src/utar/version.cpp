/*! version.cpp */

#include <utar/version.hpp>

#include <zlib.h>
#include <bzlib.h>

#include <string.h>

#define __STR_(x) #x
#define __STR(x) __STR_(x)

namespace {

char buff[4097];

}//namespace

namespace utar {

char const* version()
{
    if (buff[0] == '\0') {
        ::strcat(buff, "utar v" __STR(UTAR_VER_STRING));
        ::strcat(buff, " (zlib: ");
        ::strcat(buff, zlibVersion());
        ::strcat(buff, "; bzip2: ");
        ::strcat(buff, BZ2_bzlibVersion());
        ::strcat(buff, ")");
    }
    return buff;
}

}//namespace utar
