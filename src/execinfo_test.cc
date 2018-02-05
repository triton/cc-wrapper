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
#include <string_view>

#include "execinfo.h"
#include "execinfo_test_util.h"

namespace {

using std::string_view;

TEST(ExecinfoTest, GetExecInfoFull) {
  EXPECT_EQ(string_view("gcc"),
            string_view(get_exec_info(exec_infos, "gcc")->name));
  EXPECT_EQ(string_view("ld.gold"),
            string_view(get_exec_info(exec_infos, "ld.gold")->name));
  EXPECT_EQ(string_view("ld.bfd"),
            string_view(get_exec_info(exec_infos, "ld.bfd")->name));
}

TEST(ExecinfoTest, GetExecInfoFuzzy) {
  EXPECT_EQ(string_view("ld.bfd"),
            string_view(get_exec_info(exec_infos, "ld")->name));
}

TEST(ExecinfoTest, GetExecInfoNone) {
  EXPECT_EQ(nullptr, get_exec_info(exec_infos, "gc"));
  EXPECT_EQ(nullptr, get_exec_info(exec_infos, "gccp"));
  EXPECT_EQ(nullptr, get_exec_info(exec_infos, "ld."));
  EXPECT_EQ(nullptr, get_exec_info(exec_infos, "ld.bf"));
}

}  // namespace
