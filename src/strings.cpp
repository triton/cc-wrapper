#include <strings.hpp>

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

bool startsWith(nonstd::string_view str, nonstd::string_view prefix) {
  return str.substr(0, prefix.size()) == prefix;
}

bool endsWith(nonstd::string_view str, nonstd::string_view suffix) {
  if (str.size() < suffix.size())
    return false;
  return str.substr(str.size() - suffix.size(), suffix.size()) == suffix;
}

}  // namespace strings
}  // namespace cc_wrapper
