/*
 * Copyright 2017 cc-wrapper authors
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
#include <string_view>

#include "path.h"

namespace {

using std::string_view;

TEST(PathTest, PathBaseSplit) {
  EXPECT_EQ(-1, path_base_split("nosplit"));
  EXPECT_EQ(-1, path_base_split("no-split!@#$%^&*()"));
  EXPECT_EQ(0, path_base_split("/root"));
  EXPECT_EQ(3, path_base_split("src/binary"));
  EXPECT_EQ(5, path_base_split("/root/sub"));
  EXPECT_EQ(15, path_base_split("/this/is/a/long/one"));
}

TEST(PathTest, PathBase) {
  EXPECT_EQ(string_view("nosplit"), string_view(path_base("nosplit")));
  EXPECT_EQ(string_view("root"), string_view(path_base("/root")));
  EXPECT_EQ(string_view("binary"), string_view(path_base("src/binary")));
}

}  // namespace
