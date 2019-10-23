#include <catch2/catch.hpp>

#include <gcc/harden.hpp>

namespace cc_wrapper {
namespace gcc {
namespace harden {

TEST_CASE("Enabled flags", "[isValidFlag]") {
  Env env;
  env.position_independent = true;
  env.no_strict_overflow = true;
  env.fortify_source = true;
  env.stack_protector = true;
  env.optimize = true;
  env.lto = true;

  CHECK(isValidFlag("-Wl,-rpath", env));
  CHECK(isValidFlag("-c", env));
  CHECK(isValidFlag("-o", env));
  CHECK(isValidFlag("main.o", env));
  CHECK(!isValidFlag("-fPIC", env));
  CHECK(!isValidFlag("-fpic", env));
  CHECK(!isValidFlag("-fPIE", env));
  CHECK(!isValidFlag("-no-pie", env));
  CHECK(!isValidFlag("-pie", env));
  CHECK(!isValidFlag("-fstrict-overflow", env));
  CHECK(isValidFlag("-fstrict-overflow-hi", env));
  CHECK(!isValidFlag("-fno-stack-protector", env));
  CHECK(!isValidFlag("-fstack-protector-always", env));
  CHECK(!isValidFlag("-U_FORTIFY_SOURCE", env));
  CHECK(isValidFlag("-U_FORTIFY_SOURCE=1", env));
  CHECK(isValidFlag("-U_FORTIFY_SOURCE1", env));
  CHECK(!isValidFlag("-D_FORTIFY_SOURCE", env));
  CHECK(isValidFlag("-D_FORTIFY_SOURCE2", env));
  CHECK(!isValidFlag("-D_FORTIFY_SOURCE=", env));
  CHECK(!isValidFlag("-D_FORTIFY_SOURCE=1", env));
  CHECK(!isValidFlag("-O0", env));
  CHECK(!isValidFlag("-Ofast", env));
  CHECK(!isValidFlag("-Os", env));
  CHECK(!isValidFlag("-flto", env));
  CHECK(isValidFlag("-flto-hi", env));
  CHECK(!isValidFlag("-fno-lto", env));
  CHECK(!isValidFlag("-march=native", env));
  CHECK(!isValidFlag("-mtune=native", env));
}

TEST_CASE("Disabled Flags", "[isValidFlag]") {
  Env env;
  env.position_independent = false;
  env.no_strict_overflow = false;
  env.fortify_source = false;
  env.stack_protector = false;
  env.optimize = false;
  env.lto = false;

  CHECK(isValidFlag("-Wl,-rpath", env));
  CHECK(isValidFlag("-c", env));
  CHECK(isValidFlag("-o", env));
  CHECK(isValidFlag("main.o", env));
  CHECK(isValidFlag("-fPIC", env));
  CHECK(isValidFlag("-fpic", env));
  CHECK(isValidFlag("-fPIE", env));
  CHECK(isValidFlag("-no-pie", env));
  CHECK(isValidFlag("-pie", env));
  CHECK(isValidFlag("-fstrict-overflow", env));
  CHECK(isValidFlag("-fstrict-overflow-hi", env));
  CHECK(isValidFlag("-fno-stack-protector", env));
  CHECK(isValidFlag("-fstack-protector-always", env));
  CHECK(isValidFlag("-U_FORTIFY_SOURCE", env));
  CHECK(isValidFlag("-U_FORTIFY_SOURCE=1", env));
  CHECK(isValidFlag("-U_FORTIFY_SOURCE1", env));
  CHECK(isValidFlag("-D_FORTIFY_SOURCE", env));
  CHECK(isValidFlag("-D_FORTIFY_SOURCE2", env));
  CHECK(isValidFlag("-D_FORTIFY_SOURCE=", env));
  CHECK(isValidFlag("-D_FORTIFY_SOURCE=1", env));
  CHECK(isValidFlag("-O0", env));
  CHECK(isValidFlag("-Ofast", env));
  CHECK(isValidFlag("-Os", env));
  CHECK(isValidFlag("-flto", env));
  CHECK(isValidFlag("-flto-hi", env));
  CHECK(isValidFlag("-fno-lto", env));
  CHECK(!isValidFlag("-march=native", env));
  CHECK(!isValidFlag("-mtune=native", env));
}

}  // namespace harden
}  // namespace gcc
}  // namespace cc_wrapper
