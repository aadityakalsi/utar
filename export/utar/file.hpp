/*! file.hpp */

#include <utar/types.hpp>

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

class UTAR_API file
{
  public:
    file();

    ~file();

    file(file const&) = delete;
    file& operator=(file const&) = delete;

    bool is_open() const;

    void close();

    error_code create(file_mode mode, path_type utf8_path);

    error_code open(file_mode mode, path_type utf8_path);

    static error_code erase(path_type utf8_path);

    error_code size(file_size&);

    error_code read(file_off off, void* buff, size_t bytes);

    error_code write(file_off off, void const* buff, size_t bytes);

    error_code trunc(file_size size);

  private:
#if defined(_MSC_VER)
    using file_desc = void*;
#else
    using file_desc = int;
#endif
    static file_desc const INVALID;

    file_desc fd_;
};

}//namespace utar
