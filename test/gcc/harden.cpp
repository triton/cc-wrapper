#include <catch2/catch.hpp>

#include <gcc/harden.hpp>

namespace cc_wrapper {
namespace gcc {
namespace harden {

TEST_CASE("Enabled flags", "[isValidFlag]") {
  Env env;
  env.position_independent = true;
  env.optimize = true;

  CHECK(isValidFlag("-Wl,-rpath", env));
  CHECK(isValidFlag("-c", env));
  CHECK(isValidFlag("-o", env));
  CHECK(isValidFlag("main.o", env));
  CHECK(!isValidFlag("-fPIC", env));
  CHECK(!isValidFlag("-fpic", env));
  CHECK(!isValidFlag("-fPIE", env));
  CHECK(!isValidFlag("-no-pie", env));
  CHECK(!isValidFlag("-pie", env));
  CHECK(!isValidFlag("-O0", env));
  CHECK(!isValidFlag("-Ofast", env));
  CHECK(!isValidFlag("-Os", env));
  CHECK(isValidFlag("-O3", env));
  CHECK(!isValidFlag("-march=native", env));
  CHECK(!isValidFlag("-mtune=native", env));
}

TEST_CASE("Disabled Flags", "[isValidFlag]") {
  Env env;
  env.position_independent = false;
  env.optimize = false;

  CHECK(isValidFlag("-Wl,-rpath", env));
  CHECK(isValidFlag("-c", env));
  CHECK(isValidFlag("-o", env));
  CHECK(isValidFlag("main.o", env));
  CHECK(isValidFlag("-fPIC", env));
  CHECK(isValidFlag("-fpic", env));
  CHECK(isValidFlag("-fPIE", env));
  CHECK(isValidFlag("-no-pie", env));
  CHECK(isValidFlag("-pie", env));
  CHECK(isValidFlag("-O0", env));
  CHECK(isValidFlag("-Ofast", env));
  CHECK(isValidFlag("-Os", env));
  CHECK(isValidFlag("-O3", env));
  CHECK(!isValidFlag("-march=native", env));
  CHECK(!isValidFlag("-mtune=native", env));
}

}  // namespace harden
}  // namespace gcc
}  // namespace cc_wrapper
