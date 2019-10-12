#pragma once
#include <deque>
#include <nonstd/optional.hpp>
#include <nonstd/string_view.hpp>
#include <parallel_hashmap/phmap.h>
#include <string>
#include <vector>

#include "graph.hpp"

namespace cc_wrapper {
namespace linker {
namespace state {

struct Libs {
  using Order = graph::Graph<std::string>;
  using Set = phmap::flat_hash_set<std::string>;
  using Queue = std::deque<std::string>;
  using Array = std::vector<std::string>;

  void addRPath(nonstd::string_view path);
  inline const Set &getRPaths() const { return rpath_set; }
  void addLib(nonstd::string_view lib);
  inline const Queue &getLibs() const { return libs_to_lookup; }
  void addFile(nonstd::string_view file);
  inline const Queue &getFiles() const { return files_to_lookup; }
  void addLibPath(nonstd::string_view path);
  inline const Array &getLibPaths() const { return lib_paths; }

  std::vector<nonstd::string_view> resolveRequiredRPaths();
  nonstd::optional<std::string> nextUnresolved();
  nonstd::optional<std::string> lookupLib(nonstd::string_view lib) const;

private:
  Set rpath_set;
  Set required_rpath_set;
  Order required_rpath_order;
  Queue files_to_lookup;
  Queue libs_to_lookup;
  Array lib_paths;
  Set lib_path_set;
};

}  // namespace state
}  // namespace linker
}  // namespace cc_wrapper
