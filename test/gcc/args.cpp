#include <catch2/catch.hpp>
#include <nonstd/string_view.hpp>
#include <vector>

#include <gcc/args.hpp>

namespace cc_wrapper {
namespace gcc {
namespace args {

TEST_CASE("Parse empty state", "[parseState]") {
  auto state = parseState({}, true);
  CHECK(!state.linking);
  CHECK(!state.produceShlib);
  CHECK(state.stdinc);
  CHECK(state.stdincxx);
  CHECK(state.cxx);
}

TEST_CASE("Parse only flags", "[parseState]") {
  std::vector<nonstd::string_view> args = {
      "-v", "--help", "-xc", "-x", "c++",
  };
  auto state = parseState(args, true);
  CHECK(!state.linking);
  CHECK(!state.produceShlib);
  CHECK(state.stdinc);
  CHECK(state.stdincxx);
  CHECK(state.cxx);
}

TEST_CASE("Parse basic link", "[parseState]") {
  std::vector<nonstd::string_view> args = {
      "main.c",
  };
  auto state = parseState(args, false);
  CHECK(state.linking);
  CHECK(!state.produceShlib);
  CHECK(state.stdinc);
  CHECK(state.stdincxx);
  CHECK(!state.cxx);
}

TEST_CASE("Parse object only", "[parseState]") {
  std::vector<nonstd::string_view> args = {
      "-c", "-x", "c++", "-x", "-x", "c", "-x", "-xc", "main.c",
  };
  auto state = parseState(args, false);
  CHECK(!state.linking);
  CHECK(!state.produceShlib);
  CHECK(state.stdinc);
  CHECK(state.stdincxx);
  CHECK(state.cxx);
}

TEST_CASE("Parse preprocess only", "[parseState]") {
  std::vector<nonstd::string_view> args = {
      "-E",
      "-xc++",
      "c",
      "main.c",
  };
  auto state = parseState(args, false);
  CHECK(!state.linking);
  CHECK(!state.produceShlib);
  CHECK(state.stdinc);
  CHECK(state.stdincxx);
  CHECK(state.cxx);
}

TEST_CASE("Parse shared nostd", "[parseState]") {
  std::vector<nonstd::string_view> args = {
      "-shared", "-nostdinc", "-nostdincxx", "-x", "c", "c++", "main.c",
  };
  auto state = parseState(args, true);
  CHECK(state.linking);
  CHECK(state.produceShlib);
  CHECK(!state.stdinc);
  CHECK(!state.stdincxx);
  CHECK(!state.cxx);
}

}  // namespace args
}  // namespace gcc
}  // namespace cc_wrapper
