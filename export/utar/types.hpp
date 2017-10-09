/*! types.hpp */

#include <utar/exports.h>

namespace utar
{

using error_code = int;
using path_type = char const*;

UTAR_API
char const* error_msg(error_code code);

}//namespace utar
