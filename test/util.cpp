#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <cstdlib>
#include <nonstd/span.hpp>
#include <nonstd/string_view.hpp>
#include <string>
#include <vector>

#include "mock_exec.hpp"
#include "util.hpp"

namespace cc_wrapper {
namespace util {
namespace detail {

TEST_CASE("Strings are appended", "[append]") {
  char v[4];
  char *o = append(v, "");
  CHECK(v + 1 == o);
  CHECK(v[0] == '\0');
  o = append(o, "hi");
  CHECK(v[0] == '\0');
  CHECK(v[1] == 'h');
  CHECK(v[2] == 'i');
  CHECK(v[3] == '\0');
}

TEST_CASE("Remove trailing slashes", "[removeTrailing]") {
  CHECK(removeTrailing("") == "");
  CHECK(removeTrailing("/") == "/");
  CHECK(removeTrailing("//") == "/");
  CHECK(removeTrailing("a") == "a");
  CHECK(removeTrailing("a//") == "a");
  CHECK(removeTrailing("//a//") == "//a");
}

}  // namespace detail

TEST_CASE("Missing env", "[getenv]") {
  unsetenv("TESTVAR");
  CHECK(::getenv("TESTVAR") == nullptr);
  CHECK(getenv("TESTVAR") == nonstd::nullopt);
}

TEST_CASE("Valid env", "[getenv]") {
  setenv("TESTVAR", "test1", 1);
  CHECK(::getenv("TESTVAR") != nullptr);
  CHECK(getenv("TESTVAR") == "test1");
  unsetenv("TESTVAR1");
  CHECK(::getenv("TESTVAR1") == nullptr);
  CHECK(!getenv("TESTVAR1"));
  unsetenv("TESTVA");
  CHECK(::getenv("TESTVA") == nullptr);
  CHECK(!getenv("TESTVA"));
}

TEST_CASE("Empty env", "[getenv]") {
  setenv("TESTVAR", "", 1);
  CHECK(::getenv("TESTVAR") != nullptr);
  CHECK(getenv("TESTVAR") == "");
}

TEST_CASE("Relative Dirname", "[dirname]") {
  CHECK(dirname("") == ".");
  CHECK(dirname(".") == ".");
  CHECK(dirname("./a") == ".");
  CHECK(dirname("./") == ".");
  CHECK(dirname("abc/.") == "abc");
  CHECK(dirname("abc") == ".");
  CHECK(dirname("abc/") == ".");
  CHECK(dirname("a//b//c") == "a//b");
  CHECK(dirname("a//b//c//") == "a//b");
}

TEST_CASE("Absolute Dirname", "[dirname]") {
  CHECK(dirname("/") == "/");
  CHECK(dirname("/.") == "/");
  CHECK(dirname("/root") == "/");
  CHECK(dirname("/root/") == "/");
  CHECK(dirname("/root/.") == "/root");
}

TEST_CASE("Relative Basename", "[basename]") {
  CHECK(basename("") == "");
  CHECK(basename(".") == ".");
  CHECK(basename("./a") == "a");
  CHECK(basename("./") == ".");
  CHECK(basename("abc/.") == ".");
  CHECK(basename("abc") == "abc");
  CHECK(basename("abc/") == "abc");
  CHECK(basename("a//b//c") == "c");
  CHECK(basename("a//b//c//") == "c");
}

TEST_CASE("Absolute Basename", "[basename]") {
  CHECK(basename("/") == "/");
  CHECK(basename("/.") == ".");
  CHECK(basename("/root") == "root");
  CHECK(basename("/root/") == "root");
  CHECK(basename("/root/.") == ".");
}

TEST_CASE("Exec all options", "[exec]") {
  exec("", nonstd::span<const nonstd::string_view>{});
  CHECK(gpath == "");
  CHECK(gargv.empty());

  std::vector<std::string> expected_args{
      "arg1",
      "arg2",
  };
  std::vector<nonstd::string_view> args(expected_args.begin(),
                                        expected_args.end());
  exec("./run", args);
  CHECK(gpath == "./run");
  CHECK(gargv == expected_args);
}

}  // namespace util
}  // namespace cc_wrapper
