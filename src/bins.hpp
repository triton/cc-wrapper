#pragma once
#include <functional>
#include <memory>
#include <nonstd/optional.hpp>
#include <nonstd/string_view.hpp>
#include <parallel_hashmap/phmap.h>
#include <utility>
#include <vector>

namespace cc_wrapper {
namespace bins {

enum class Type { GENERIC, GCC_COMPILER, GXX_COMPILER, GCC_WRAPPER, LINKER };

using Name = nonstd::string_view;
using Path = nonstd::string_view;
using Flag = nonstd::string_view;

struct Info {
  Name name;
  Type type;
  std::vector<Flag> extra_args;

  inline Info(Name name, Type type, std::vector<Flag> &&extra_args)
      : name(name), type(type), extra_args(std::move(extra_args)) {}
};

struct GccInfo : public Info {
  nonstd::optional<Flag> prefix_map_flag;

  inline GccInfo(Name name, Type type, std::vector<Flag> &&extra_args,
                 nonstd::optional<Flag> &&prefix_map_flag)
      : Info(name, type, std::move(extra_args)),
        prefix_map_flag(prefix_map_flag) {}
};

using InfoMap = phmap::flat_hash_map<Name, std::unique_ptr<Info>,
                                     phmap::Hash<nonstd::string_view>,
                                     phmap::EqualTo<nonstd::string_view>>;

const InfoMap &getInfoMap();

}  // namespace bins
}  // namespace cc_wrapper
