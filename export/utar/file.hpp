/*! file.hpp */

#include <utar/types.hpp>

#if defined(_MSC_VER)
#  define NOMINMAX
#  define WIN32_LEAN_AND_MEAN
#  include <Windows.h>
#  define _UTAR_FILE_TYPE_ HANDLE
#  define _UTAR_INVALID_FILE_ INVALID_HANDLE_VALUE
#else
#  include <fcntl.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <unistd.h>
#  define _UTAR_FILE_TYPE_ int
#  define _UTAR_INVALID_FILE_ -1
#endif

#include <cassert>
#include <errno.h>
#include <limits.h>

namespace utar
{

using file_size = int64_t;
using file_off = uint64_t;

enum class file_mode
{
    // Open the file for sequential reads
    scan,
    // Open the file for random reads
    read,
    // Open the file for random reads and appending writes
    append,
    // Open the file for random reads and writes
    write
};

class file
{
  public:
    file() : fd_(INVALID)
    { }

    ~file()
    {
        close();
    }

    file(file const&) = delete;
    file& operator=(file const&) = delete;

    bool is_open() const
    {
        return fd_ != INVALID;
    }

    void close();

    error_code create(file_mode mode, path_type utf8_path);

    error_code open(file_mode mode, path_type utf8_path);

    static error_code erase(path_type utf8_path);

    error_code size(file_size&);

    error_code read(file_off off, void* buff, size_t bytes);

    error_code write(file_off off, void const* buff, size_t bytes);

    error_code trunc(file_size size);

  private:
    using file_desc = _UTAR_FILE_TYPE_;
    static file_desc const INVALID = _UTAR_INVALID_FILE_;

    file_desc fd_;
};

#if defined(_MSC_VER)

void file::close()
{
    if (fd_ != INVALID) {
        ::CloseHandle(fd_);
        fd_ = INVALID;
    }
}

namespace {

void get_mode_advice(file_mode mode, int& flags, int& advice)
{
    switch(mode) {
    case file_mode::scan:
        flags = GENERIC_READ;
        advice = FILE_FLAG_SEQUENTIAL_SCAN;
        break;
    case file_mode::read:
        flags = GENERIC_READ;
        advice = FILE_FLAG_RANDOM_ACCESS;
        break;
    case file_mode::append:
        flags = GENERIC_READ | GENERIC_WRITE;
        advice = FILE_FLAG_RANDOM_ACCESS;
        break;
    case file_mode::write:
        flags = GENERIC_READ | GENERIC_WRITE;
        advice = FILE_FLAG_RANDOM_ACCESS;
        break;
    }
}

wchar_t* utf16_filename(path_type p)
{
    static wchar_t BUFF[4097];
    int inp_len = ::strlen(p);
    int len = MultiByteToWideChar(CP_UTF8, 0, p, inp_len, NULL, 0);
    if (len > 0) {
        MultiByteToWideChar(CP_UTF8, 0, p, inp_len, BUFF, 4096);
    }
    BUFF[len] = wchar_t(0);
    return BUFF;
}

}//namespace

error_code file::create(file_mode mode, path_type utf8_path)
{
    assert(!is_open());
    int flags, advice;
    get_mode_advice(mode, flags, advice);
    fd_ = ::CreateFileW(utf16_filename(utf8_path), flags, 0, NULL, CREATE_NEW, advice, NULL);
    if (fd_ == INVALID) {
        return errno;
    }
    return 0;
}

error_code file::open(file_mode mode, path_type utf8_path)
{
    assert(!is_open());
    int flags, advice;
    get_mode_advice(mode, flags, advice);
    fd_ = ::CreateFileW(utf16_filename(utf8_path), flags, 0, NULL, OPEN_EXISTING, advice, NULL);
    if (fd_ == INVALID) {
        return errno;
    }
    return 0;
}

error_code file::erase(path_type utf8_path)
{
    if (!::DeleteFileW(utf16_filename(utf8_path))) {
        return errno;
    }
    return 0;
}

error_code file::size(file_size& size)
{
    assert(is_open());
    LARGE_INTEGER file_size;
    if (!::GetFileSizeEx(hf_, &file_size)) {
        return errno;
    }
    size = file_size.QuadPart;
    return 0;
}

error_code file::read(file_off off, void* buff, size_t bytes)
{
    while (bytes > 0) {
        DWORD amt;
        LARGE_INTEGER off_li;
        OVERLAPPED ov;

        off_li.QuadPart = static_cast<LONGLONG>(off);
        ov.Offset = off_li.LowPart;
        ov.OffsetHigh = off_li.HighPart;
        ov.hEvent = NULL;

        if (!::ReadFile(fd_, buff, bytes > INT_MAX ? INT_MAX : bytes, &amt, &ov)) {
            return errno;
        }

        assert(amt > 0);

        off += amt;
        bytes -= amt;
        buff = reinterpret_cast<char*>(buff) + amt;
    }
    return 0;
}

error_code file::write(file_off off, void const* buff, size_t bytes)
{
    while (bytes > 0) {
        DWORD amt;
        LARGE_INTEGER off_li;
        OVERLAPPED ov;

        off_li.QuadPart = static_cast<LONGLONG>(off);
        ov.Offset = off_li.LowPart;
        ov.OffsetHigh = off_li.HighPart;
        ov.hEvent = NULL;

        if (!::WriteFile(fd_, buff, bytes > INT_MAX ? INT_MAX : bytes, &amt, &ov)) {
            return errno;
        }

        assert(amt > 0);

        off += amt;
        bytes -= amt;
        buff = reinterpret_cast<char*>(buff) + amt;
    }
    return 0;
}

error_code file::trunc(file_size size)
{
    LARGE_INTEGER off;
    off.QuadPart = size;
    if (!::SetFilePointerEx(fd_, off, NULL, FILE_BEGIN) || !::SetEndOfFile(fd_)) {
        return errno;
    }
    return 0;
}

#else

void file::close()
{
    if (fd_ != INVALID) {
        ::close(fd_);
        fd_ = INVALID;
    }
}

namespace {

void get_mode_advice(file_mode mode, int& flags, int& advice)
{
    switch(mode) {
    case file_mode::scan:
        flags = O_RDONLY;
#ifndef __APPLE__
        advice = POSIX_FADV_SEQUENTIAL;
#endif
        break;
    case file_mode::read:
        flags = O_RDONLY;
#ifndef __APPLE__
        advice = POSIX_FADV_RANDOM;
#endif
        break;
    case file_mode::append:
        flags = O_RDWR | O_APPEND;
#ifndef __APPLE__
        advice = POSIX_FADV_RANDOM;
#endif
        break;
    case file_mode::write:
        flags = O_RDWR;
#ifndef __APPLE__
        advice = POSIX_FADV_NORMAL;
#endif
        break;
    }
}

}//namespace

error_code file::create(file_mode mode, path_type utf8_path)
{
    assert(!is_open());
    int flags = 0, advice = -1;
    get_mode_advice(mode, flags, advice);
    fd_ = ::open(utf8_path, flags | O_CREAT | O_EXCL, 0644);
    if (fd_ == INVALID) {
        return errno;
    }
#ifndef __APPLE__
    if (::posix_fadvise(fd_, 0, 0, advice) != 0) {
        return errno;
    }
#endif
}

error_code file::open(file_mode mode, path_type utf8_path)
{
    assert(!is_open());
    int flags = 0, advice = -1;
    get_mode_advice(mode, flags, advice);
    fd_ = ::open(utf8_path, flags, 0644);
    if (fd_ == INVALID) {
        return errno;
    }
#ifndef __APPLE__
    if (::posix_fadvise(fd_, 0, 0, advice) != 0) {
        return errno;
    }
#endif
}

error_code file::erase(path_type utf8_path)
{
    return ::unlink(utf8_path) ? errno : 0;
}

error_code file::size(file_size& sz)
{
    static_assert(sizeof(stat::st_size) == sizeof(file_size), "");
    struct stat st;
    if (::fstat(fd_, &st) != 0) {
        return errno;
    }
    sz = st.st_size;
    return 0;
}

error_code file::read(file_off off, void* buff, size_t bytes)
{
    static_assert(sizeof(file_off) >= sizeof(file_off), "");
    while (bytes > 0) {
        auto amt = ::pread(fd_, buff, bytes > INT_MAX ? INT_MAX : bytes, off);
        if (amt == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                return errno;
            }
        }
        assert(amt > 0);
        off += amt;
        bytes -= amt;
        buff = reinterpret_cast<char*>(buff) + amt;
    }
    return 0;
}

error_code file::write(file_off off, void const* buff, size_t bytes)
{
    while (bytes > 0) {
        auto amt = ::pwrite(fd_, buff, bytes > INT_MAX ? INT_MAX : bytes, off);
        if (amt == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                return errno;
            }
        }
        off += amt;
        bytes -= amt;
        buff = reinterpret_cast<char const*>(buff) + amt;
    }
    return 0;
}

error_code file::trunc(file_size size)
{
    error_code err = 0;
    while (1) {
        if (::ftruncate(fd_, size) == 0) {
            break;
        } else if (errno == EINTR) {
            continue;
        } else {
            err = errno;
            break;
        }
    }
    return err;
}

#endif

}//namespace utar
