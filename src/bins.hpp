#pragma once
#include <functional>
#include <nonstd/optional.hpp>
#include <nonstd/string_view.hpp>
#include <parallel_hashmap/phmap.h>
#include <vector>

namespace cc_wrapper {
namespace bins {

enum class Type { GENERIC, GCC_COMPILER, GXX_COMPILER, LINKER };

using Name = nonstd::string_view;
using Path = nonstd::string_view;
using Flag = nonstd::string_view;

struct Info {
  Name name;
  Type type;
  Path abs_path;
  std::vector<nonstd::string_view> extra_args;
  nonstd::optional<Flag> prefix_map_flag;
};

using InfoMap =
    phmap::flat_hash_map<Name, Info, phmap::Hash<nonstd::string_view>,
                         phmap::EqualTo<nonstd::string_view>>;

const InfoMap &getInfoMap();

}  // namespace bins
}  // namespace cc_wrapper
