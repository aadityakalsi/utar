/*! file.cpp */

#include "defs.h"

#include <utar/version.hpp>
#include <utar/file.hpp>

#include <errno.h>
#include <stdio.h>
#include <string.h>


void fCreateOpenReadWrite()
{
    utar::file f;
    testThat(!f.is_open());
    testThat(f.create(utar::file_mode::write, "foo.test") == 0);
    testThat(f.is_open());
    testThat(f.write(0, "foo", 3) == 0);
    f.close();
    testThat(!f.is_open());
    testThat(f.open(utar::file_mode::scan, "foo.test") == 0);
    char buf[4];
    testThat(f.read(0, buf, 3) == 0);
    buf[3] = '\0';
    testThat(::strcmp(buf, "foo") == 0);
    f.close();
    testThat(utar::file::erase("foo.test") == 0);
}

setupSuite(file)
{
    addTest(fCreateOpenReadWrite);
}

void verPrint()
{
    ::printf("%s: ", utar::version());
}

void errStr()
{
    ::printf("%s: ", utar::error_msg(1));
    ::printf("%s: ", utar::error_msg(-ENOMEM));
}

setupSuite(version)
{
    addTest(verPrint);
    addTest(errStr);
}
