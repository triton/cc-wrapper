#include "strings.hpp"

namespace cc_wrapper {
namespace strings {

nonstd::string_view split(nonstd::string_view &str, char sep) {
  size_t pos = str.find(sep);
  auto first = str.substr(0, pos);
  if (pos == nonstd::string_view::npos)
    str = "";
  else
    str = str.substr(pos + 1);
  return first;
}

}  // namespace strings
}  // namespace cc_wrapper
