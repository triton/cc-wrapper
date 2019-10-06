#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <vector>

#include "env.hpp"
#include "linker/path.hpp"
#include "path.hpp"

namespace cc_wrapper {
namespace linker {
namespace path {

SCENARIO("Append Good Flags", "[appendGood]") {
  env::detail::enforcingPurity() = true;
  std::vector<nonstd::string_view> output;
  const std::vector<nonstd::string_view> prefixes = {"/build"};

  SECTION("Appends to output without validation") {
    output.push_back("-dynamic-linker=/usr/lib/ld.so");
    const std::vector<nonstd::string_view> expected = {
        "-dynamic-linker=/usr/lib/ld.so",
    };
    appendGood(output, {}, prefixes);
    CHECK(expected == output);
  }
  SECTION("Empty string works") {
    const std::vector<nonstd::string_view> expected = {""};
    const std::vector<nonstd::string_view> input = {""};
    appendGood(output, input, prefixes);
    CHECK(expected == output);
  }
  SECTION("Ignoring purity for plugins") {
    // clang-format off
    const std::vector<nonstd::string_view> expected = {
      "-plugin", "/usr/lib/liblto.so",
      "-plugin-opt", "/usr/lib/liblto.so",
      "-plugin=/usr/lib/liblto.so",
    };
    const std::vector<nonstd::string_view> input = {
      "-plugin", "/usr/lib/liblto.so",
      "-plugin-opt", "/usr/lib/liblto.so",
      "-plugin=/usr/lib/liblto.so",
    };
    // clang-format on
    appendGood(output, input, prefixes);
    CHECK(expected == output);
  }
  SECTION("Longer flags filter") {
    // clang-format off
    const std::vector<nonstd::string_view> input = {
      "-rpath=/usr/lib1",
      "-rpath", "/usr/lib2",
      "--rpath", "/usr/lib3",
    };
    // clang-format on
    appendGood(output, input, prefixes);
    CHECK(output.empty());
  }
  SECTION("Longer flags okay") {
    // clang-format off
    const std::vector<nonstd::string_view> expected = {
      "-rpat", "/build/lib4",
      "-rpath", "/build/lib5",
      "-rpath=/build/lib5",
    };
    const std::vector<nonstd::string_view> input = {
      "-rpat", "/build/lib4",
      "-rpath", "/build/lib5",
      "-rpath=/build/lib5",
    };
    // clang-format on
    appendGood(output, input, prefixes);
    CHECK(expected == output);
  }
  SECTION("Short flags filter") {
    // clang-format off
    const std::vector<nonstd::string_view> input = {
      "-L/usr/lib",
      "-L", "/usr/lib",
    };
    // clang-format on
    appendGood(output, input, prefixes);
    CHECK(output.empty());
  }
  SECTION("Short flags okay") {
    // clang-format off
    const std::vector<nonstd::string_view> expected = {
      "-L/build/lib",
      "-L=/usr/lib",
      "-L", "/build/lib",
    };
    const std::vector<nonstd::string_view> input = {
      "-L/build/lib",
      "-L=/usr/lib",
      "-L", "/build/lib",
    };
    // clang-format on
    appendGood(output, input, prefixes);
    CHECK(expected == output);
  }
  SECTION("Non-file flags") {
    // clang-format off
    const std::vector<nonstd::string_view> expected = {
      "-rpat", "/build/lib",
      "-z", "relro",
    };
    const std::vector<nonstd::string_view> input = {
      "-rpat", "/build/lib",
      "-z", "relro",
    };
    // clang-format on
    appendGood(output, input, prefixes);
    CHECK(expected == output);
  }
  SECTION("Dynamic linkers okay") {
    // clang-format off
    const std::vector<nonstd::string_view> expected = {
      "-dynamic-linker", "/build/lib/ld.so",
      "-dynamic-linker=/build/lib/ld.so",
    };
    const std::vector<nonstd::string_view> input = {
      "-dynamic-linker", "/build/lib/ld.so",
      "-dynamic-linker=/build/lib/ld.so",
    };
    // clang-format on
    appendGood(output, input, prefixes);
    CHECK(expected == output);
  }
  SECTION("Dynamic linkers error split") {
    // clang-format off
    const std::vector<nonstd::string_view> input = {
      "-dynamic-linker", "/usr/lib/ld.so",
    };
    // clang-format on
    CHECK_THROWS_AS(appendGood(output, input, prefixes),
                    cc_wrapper::path::PurityError);
  }
  SECTION("Dynamic linkers error combined") {
    // clang-format off
    const std::vector<nonstd::string_view> input = {
      "-dynamic-linker=/usr/lib/ld.so",
    };
    // clang-format on
    CHECK_THROWS_AS(appendGood(output, input, prefixes),
                    cc_wrapper::path::PurityError);
  }
  SECTION("Files okay") {
    // clang-format off
    const std::vector<nonstd::string_view> expected = {
      "main.o",
      "/build/lib/crtn.o",
    };
    const std::vector<nonstd::string_view> input = {
      "main.o",
      "/build/lib/crtn.o",
    };
    // clang-format on
    appendGood(output, input, prefixes);
    CHECK(expected == output);
  }
  SECTION("File error") {
    // clang-format off
    const std::vector<nonstd::string_view> input = {
      "/usr/lib/crtn.o",
    };
    // clang-format on
    CHECK_THROWS_AS(appendGood(output, input, prefixes),
                    cc_wrapper::path::PurityError);
  }
  SECTION("File non-path flag error") {
    // clang-format off
    const std::vector<nonstd::string_view> input = {
      "-rpat", "/usr/lib",
    };
    // clang-format on
    CHECK_THROWS_AS(appendGood(output, input, prefixes),
                    cc_wrapper::path::PurityError);
  }
}

}  // namespace path
}  // namespace linker
}  // namespace cc_wrapper
