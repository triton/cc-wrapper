#include <fcntl.h>
#include <fmt/format.h>
#include <nonstd/string_view.hpp>
#include <string>
#include <utility>

#include "../path.hpp"
#include "env.hpp"
#include "file.hpp"
#include "fmt_sv.hpp"
#include "linker/script.hpp"
#include "linker/state.hpp"
#include "strings.hpp"
#include "util.hpp"

namespace cc_wrapper {
namespace linker {
namespace state {

void Libs::addRPath(nonstd::string_view path) {
  rpath_set.insert(path::canonicalize(path));
}

void Libs::addLib(nonstd::string_view lib) {
  libs_to_lookup.push_back(lib.to_string());
}

void Libs::addFile(nonstd::string_view file) {
  files_to_lookup.push_back(file.to_string());
}

void Libs::addLibPath(nonstd::string_view path) {
  auto path_s = path::canonicalize(path);
  if (lib_path_set.find(path_s) != lib_path_set.end())
    return;
  lib_paths.push_back(path_s);
  lib_path_set.insert(std::move(path_s));
}

std::vector<nonstd::string_view> Libs::resolveRequiredRPaths() {
  while (auto lib = nextUnresolved()) {
    if (env::isDebug()) {
      fmt::print(stderr, "cc-wrapper: Checking lib `{}`\n", *lib);
    }
    file::Fd fd(lib->c_str(), O_RDONLY);
    if (script::isScript(fd)) {
      if (env::isDebug()) {
        fmt::print(stderr, "cc-wrapper: Parsing script `{}`\n", *lib);
      }
      auto cwd = path::isAbsolute(*lib) ? util::dirname(*lib) : ".";
      script::parseLibs(*this, fd, cwd);
      continue;
    }
    if (util::basename(*lib).find(".so") == nonstd::string_view::npos)
      continue;
    if (env::isDebug()) {
      fmt::print(stderr, "cc-wrapper: Determining RPaths for lib `{}`\n", *lib);
    }
    while (auto lnk = file::readlinkCanonicalized(lib->c_str())) {
      auto libdir = util::dirname(*lib);
      auto lnkdir = util::dirname(*lnk);
      if (libdir != lnkdir)
        required_rpath_order.connectAndInsert(libdir.to_string(),
                                              lnkdir.to_string());
      else
        required_rpath_order.insert(libdir.to_string());
      *lib = std::move(*lnk);
    }
    required_rpath_set.insert(util::dirname(*lib).to_string());
  }

  auto possible_rpaths = required_rpath_order.toposort();
  std::vector<nonstd::string_view> ret;
  ret.reserve(possible_rpaths.size());
  for (auto rpath : possible_rpaths) {
    if (required_rpath_set.find(rpath) == required_rpath_set.end())
      continue;
    if (rpath_set.find(rpath) != rpath_set.end())
      continue;
    ret.push_back(rpath.get());
  }
  return ret;
}

nonstd::optional<std::string> Libs::nextUnresolved() {
  if (!files_to_lookup.empty()) {
    std::string ret = std::move(files_to_lookup.front());
    files_to_lookup.pop_front();
    return ret;
  }

  if (!libs_to_lookup.empty()) {
    const auto &lib = libs_to_lookup.front();
    auto file = lookupLib(lib);
    if (!file)
      throw std::runtime_error(strings::cat("Failed to resolve -l", lib));
    libs_to_lookup.pop_front();
    return file;
  }

  return nonstd::nullopt;
}

nonstd::optional<std::string> Libs::lookupLib(nonstd::string_view lib) const {
  std::string file;
  for (const auto &path : lib_paths) {
    file.clear();
    strings::append(file, path, "/lib", lib, ".so");
    if (file::exists(file.c_str()))
      return file;

    file.resize(file.size() - 3);
    strings::append(file, ".a");
    if (file::exists(file.c_str()))
      return file;
  }

  return nonstd::nullopt;
}

}  // namespace state
}  // namespace linker
}  // namespace cc_wrapper
