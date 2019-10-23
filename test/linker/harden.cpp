#include <catch2/catch.hpp>

#include <linker/harden.hpp>

namespace cc_wrapper {
namespace linker {
namespace harden {

Env testEnv(bool b) {
  Env env;
  env.new_dtags = b;
  env.no_exec_stack = b;
  env.relro = b;
  env.bind_now = b;
  return env;
}

TEST_CASE("Doesn't filter", "[filterFlags]") {
  // clang-format off
  std::vector<nonstd::string_view> expected;
  std::vector<nonstd::string_view> old_args = {
      "--enable-new-dtags",
      "--disable-new-dtags",
      "-zlazy",
      "-z", "now",
      "-o", "main",
      "-z", "-z", "now",
      "-z", "norelro",
      "-z", "execstack",
      "main.o",
      "-z",
  };
  std::vector<nonstd::string_view> new_args = {
    "-z", "now",
  };
  // clang-format on
  Env env;
  SECTION("When disabled") {
    env = testEnv(false);
    // clang-format off
    expected = {
        "-z", "now",
        "--enable-new-dtags",
        "--disable-new-dtags",
        "-zlazy",
        "-z", "now",
        "-o", "main",
        "-z", "-z", "now",
        "-z", "norelro",
        "-z", "execstack",
        "main.o",
        "-z",
    };
    // clang-format on
  }
  SECTION("When enabled") {
    env = testEnv(true);
    // clang-format off
    expected = {
        "-z", "now",
        "-o", "main",
        "-z", "-z", "now",
        "main.o",
        "-z",
    };
    // clang-format on
  }
  filterFlags(new_args, old_args, env);
  CHECK(new_args == expected);
}

}  // namespace harden
}  // namespace linker
}  // namespace cc_wrapper
