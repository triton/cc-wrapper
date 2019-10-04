#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <vector>

#include "env.hpp"
#include "path.hpp"

namespace cc_wrapper {
namespace path {

TEST_CASE("Impure", "[isValid]") {
  env::detail::enforcingPurity() = false;
  CHECK(isValid("./local/file", {}));
  CHECK(isValid("local", {}));
  CHECK(!isValid("/no-such-path", {}));
  CHECK(!isValid("/no-such-path/file", {}));
  CHECK(isValid("/usr/bin/bash", {}));
}

TEST_CASE("Pure", "[isValid]") {
  env::detail::enforcingPurity() = true;
  CHECK(isValid("./local/file", {}));
  CHECK(isValid("local", {}));
  CHECK(!isValid("/no-such-path", {}));
  CHECK(!isValid("/no-such-path/file", {}));
  CHECK(!isValid("/usr/bin/bash", {}));
  const std::vector<nonstd::string_view> prefixes = {"/usr", "/build"};
  CHECK(isValid("/usr/bin/bash", prefixes));
  CHECK(isValid("/build/bash", prefixes));
  CHECK(!isValid("/tmp/bash", prefixes));
}

}  // namespace path
}  // namespace cc_wrapper
