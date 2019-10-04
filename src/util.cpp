#include <cstdlib>
#include <cstring>
#include <fmt/format.h>
#include <memory>
#include <unistd.h>

#include "config.h"
#include "strings.hpp"
#include "util.hpp"

extern char **environ;

namespace cc_wrapper {
namespace util {

namespace detail {

char *append(char *out, nonstd::string_view str) {
  memcpy(out, str.data(), str.size());
  out[str.size()] = '\0';
  return out + str.size() + 1;
}

nonstd::string_view removeTrailing(nonstd::string_view path) {
  while (path.size() > 1 && path[path.size() - 1] == '/')
    path.remove_suffix(1);
  return path;
}

bool &enforcingPurity() {
  static bool val = getenv(VAR_PREFIX "_ENFORCE_PURITY") == "1";
  return val;
}

}  // namespace detail

nonstd::optional<nonstd::string_view> getenv(nonstd::string_view var) {
  for (const char *const *env = environ; *env != nullptr; env++) {
    nonstd::string_view val = *env;
    if (var == strings::split(val, '='))
      return val;
  }
  return nonstd::nullopt;
}

nonstd::string_view dirname(nonstd::string_view path) {
  path = detail::removeTrailing(path);
  const size_t pos = path.rfind('/');
  if (pos == 0)
    return "/";
  if (pos == nonstd::string_view::npos)
    return ".";
  return detail::removeTrailing(path.substr(0, pos));
}

nonstd::string_view basename(nonstd::string_view path) {
  path = detail::removeTrailing(path);
  const size_t pos = path.rfind('/');
  if (pos == nonstd::string_view::npos || path.size() < 2)
    return path;
  return path.substr(pos + 1);
}

static void execv(const char *path, char *argv[]) {
  if (isDebug()) {
    fmt::print(stderr, "cc-wrapper: Exec arguments to `{}`:\n", path);
    for (char **arg = argv; *arg != nullptr; ++arg)
      fmt::print(stderr, "cc-wrapper:   {}\n", *arg);
    fmt::print(stderr, "\n");
  }
  ::execv(path, argv);
}

void exec(nonstd::string_view bin,
          nonstd::span<const nonstd::string_view> args) {
  const size_t nargs = args.size() + 1;
  size_t str_size = bin.size() + 1;
  for (const auto &arg : args) {
    str_size += arg.size() + 1;
  }
  std::unique_ptr<char[]> str(new char[str_size]);
  std::unique_ptr<char *[]> argv(new char *[nargs]);
  {
    char *str_iter = str.get();
    char **argv_iter = argv.get();
    str_iter = detail::append(str_iter, bin);
    for (const auto &arg : args) {
      *argv_iter = str_iter;
      str_iter = detail::append(str_iter, arg);
      argv_iter++;
    }
    *argv_iter = nullptr;
  }
  execv(str.get(), argv.get());
}

bool isDebug() {
  static bool val = getenv(VAR_PREFIX "_DEBUG") == "1";
  return val;
}

bool isEnforcingPurity() { return detail::enforcingPurity(); }

}  // namespace util
}  // namespace cc_wrapper
