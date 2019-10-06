#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <vector>

#include "env.hpp"
#include "path.hpp"

namespace cc_wrapper {
namespace path {

TEST_CASE("Impure", "[isPure]") {
  env::detail::enforcingPurity() = false;
  CHECK(isPure("./local/file", {}));
  CHECK(isPure("local", {}));
  CHECK(!isPure("/no-such-path", {}));
  CHECK(!isPure("/no-such-path/file", {}));
  CHECK(isPure("/usr/bin/bash", {}));
}

TEST_CASE("Pure", "[isPure]") {
  env::detail::enforcingPurity() = true;
  CHECK(isPure("./local/file", {}));
  CHECK(isPure("local", {}));
  CHECK(!isPure("/no-such-path", {}));
  CHECK(!isPure("/no-such-path/file", {}));
  CHECK(!isPure("/usr/bin/bash", {}));
  const std::vector<nonstd::string_view> prefixes = {"/usr", "/build"};
  CHECK(isPure("/usr/bin/bash", prefixes));
  CHECK(isPure("/build/bash", prefixes));
  CHECK(!isPure("/tmp/bash", prefixes));
}

}  // namespace path
}  // namespace cc_wrapper
