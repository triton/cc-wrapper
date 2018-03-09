/*
 * Copyright 2018 cc-wrapper authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <string>
#include <string_view>

#include "environment.h"
#include "mod_common.h"
#include "mod_test_util.hh"

namespace {

using std::string;
using std::string_view;

class ModCommonTest : public ::ModTest {};

TEST_F(ModCommonTest, RewritePathEmpty) {
  EXPECT_TRUE(mod_common_rewrite(nullptr, nullptr, env));

  ExpectEnv({{"PATH", CC_WRAPPER_BIN}});
}

TEST_F(ModCommonTest, RewritePathPopulated) {
  const char *orig_path = "/bin:/usr/bin";
  EXPECT_TRUE(environment_set(env, "PATH", orig_path));
  EXPECT_TRUE(mod_common_rewrite(nullptr, nullptr, env));

  ExpectEnv({{"PATH", string(CC_WRAPPER_BIN ":") + orig_path}});
}

}  // namespace
