#include <catch2/catch.hpp>

#include "flags.hpp"

namespace cc_wrapper {
namespace flags {

TEST_CASE("Append empty string", "[appendFromString]") {
  const std::vector<nonstd::string_view> expected = {"hi"};
  std::vector<nonstd::string_view> flags = expected;
  appendFromString(flags, "");
  CHECK(flags == expected);
}

TEST_CASE("Append single string", "[appendFromString]") {
  const std::vector<nonstd::string_view> expected = {"hi", "second"};
  std::vector<nonstd::string_view> flags = {"hi"};
  appendFromString(flags, "second");
  CHECK(flags == expected);
}

TEST_CASE("Append padded multi string", "[appendFromString]") {
  const std::vector<nonstd::string_view> expected = {"hi", "second", "third"};
  std::vector<nonstd::string_view> flags = {"hi"};
  appendFromString(flags, "   second    third ");
  CHECK(flags == expected);
}

TEST_CASE("Append flags from missing env var", "[appendFromVar]") {
  const std::vector<nonstd::string_view> expected = {"hi"};
  std::vector<nonstd::string_view> flags = {"hi"};
  unsetenv("VAR");
  appendFromVar(flags, "VAR");
  CHECK(flags == expected);
}

TEST_CASE("Append flags from env var", "[appendFromVar]") {
  const std::vector<nonstd::string_view> expected = {"hi", "str", "third"};
  std::vector<nonstd::string_view> flags = {"hi"};
  setenv("VAR", " str   third", 1);
  appendFromVar(flags, "VAR");
  CHECK(flags == expected);
}

}  // namespace flags
}  // namespace cc_wrapper
