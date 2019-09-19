#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "bins.hpp"
#include "main.hpp"
#include "mock_exec.hpp"

namespace cc_wrapper {
namespace bins {

const InfoMap &getInfoMap() {
  static InfoMap map = {
      {"true", {"true", Type::GENERIC, "/bin/true", {}, nonstd::nullopt}},
      {"false", {"false", Type::GENERIC, "/bin/true", {"-n"}, nonstd::nullopt}},
  };
  return map;
}

}  // namespace bins

TEST_CASE("No bin name", "[main]") {
  char *argv[] = {nullptr};
  CHECK(main(0, argv) == 127);
}

TEST_CASE("No cc-wrapper name", "[main]") {
  char arg0[] = "cc-wrapper";
  char *argv[] = {arg0, nullptr};
  CHECK(main(1, argv) == 127);
}

TEST_CASE("Invalid name", "[main]") {
  char arg0[] = "no";
  char *argv[] = {arg0, nullptr};
  CHECK(main(1, argv) == 127);
}

TEST_CASE("Run true", "[main]") {
  char arg0[] = "/usr/bin/true";
  char arg1[] = "yes";
  char *argv[] = {arg0, arg1, nullptr};
  CHECK(main(2, argv) == 126);
  CHECK(gpath == "/bin/true");
  REQUIRE(gargv.size() == 2);
  CHECK(gargv[0] == "/bin/true");
  CHECK(gargv[1] == arg1);
}

TEST_CASE("Run false", "[main]") {
  char arg0[] = "/usr/bin/cc-wrapper";
  char arg1[] = "false";
  char arg2[] = "no";
  char *argv[] = {arg0, arg1, arg2, nullptr};
  CHECK(main(3, argv) == 126);
  CHECK(gpath == "/bin/true");
  REQUIRE(gargv.size() == 3);
  CHECK(gargv[0] == "/bin/false");
  CHECK(gargv[1] == "-n");
  CHECK(gargv[2] == "no");
}

}  // namespace cc_wrapper
