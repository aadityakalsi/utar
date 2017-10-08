/*! exports.c */

#include "defs.h"

#include "utar/exports.h"

#ifdef UTAR_C_API
#  define VER_DEF_FOUND 1
#else
#  define VER_DEF_FOUND 0
#endif

void exportHdr()
{
    testThat(1 == VER_DEF_FOUND);
}

setupSuite(exports)
{
    addTest(exportHdr);
}
