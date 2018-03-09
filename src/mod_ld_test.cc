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

#include "arguments.h"
#include "config.h"
#include "mod_common.h"
#include "mod_ld.h"
#include "mod_test_util.hh"
#include "string-util.h"

const char *target_dl;
const char *target_libc_dynamic_libs;
const char *target_libc_static_libs;

namespace {

using std::string;
using std::string_view;

class ModLdTest : public ::ModTest {
 protected:
  void SetUp() override {
    ::ModTest::SetUp();

    target_dl = nullptr;
    target_libc_dynamic_libs = nullptr;
    target_libc_static_libs = nullptr;
  }
};

TEST_F(ModLdTest, TestUserArgsEmpty) {
  struct ld_args ld_args;
  EXPECT_TRUE(ld_args_init(args, &ld_args));
  EXPECT_EQ(0u, arguments_nelems(args));
  EXPECT_EQ(0u, ld_args.user_args_start);
  EXPECT_EQ(0u, ld_args.user_args_end);
}

TEST_F(ModLdTest, TestUserArgsNoMarkers) {
  EXPECT_TRUE(arguments_insert(args, 0, "-g"));
  EXPECT_TRUE(arguments_insert(args, 1, "-llib"));

  struct ld_args ld_args;
  EXPECT_TRUE(ld_args_init(args, &ld_args));
  EXPECT_EQ(2u, arguments_nelems(args));
  EXPECT_EQ(0u, ld_args.user_args_start);
  EXPECT_EQ(2u, ld_args.user_args_end);
  EXPECT_EQ(string_view("-g"), arguments_get(args, 0));
  EXPECT_EQ(string_view("-llib"), arguments_get(args, 1));
}

TEST_F(ModLdTest, TestUserArgsNoEnd) {
  EXPECT_TRUE(arguments_insert(args, 0, "-g"));
  EXPECT_TRUE(arguments_insert(args, 1, CC_WRAPPER_USER_ARGS_BEGIN));
  EXPECT_TRUE(arguments_insert(args, 2, "-llib"));

  struct ld_args ld_args;
  EXPECT_TRUE(ld_args_init(args, &ld_args));
  EXPECT_EQ(2u, arguments_nelems(args));
  EXPECT_EQ(1u, ld_args.user_args_start);
  EXPECT_EQ(2u, ld_args.user_args_end);
  EXPECT_EQ(string_view("-g"), arguments_get(args, 0));
  EXPECT_EQ(string_view("-llib"), arguments_get(args, 1));
}

TEST_F(ModLdTest, TestUserArgsNoBegin) {
  EXPECT_TRUE(arguments_insert(args, 0, "-g"));
  EXPECT_TRUE(arguments_insert(args, 1, CC_WRAPPER_USER_ARGS_END));
  EXPECT_TRUE(arguments_insert(args, 2, "-llib"));

  struct ld_args ld_args;
  EXPECT_TRUE(ld_args_init(args, &ld_args));
  EXPECT_EQ(2u, arguments_nelems(args));
  EXPECT_EQ(0u, ld_args.user_args_start);
  EXPECT_EQ(1u, ld_args.user_args_end);
  EXPECT_EQ(string_view("-g"), arguments_get(args, 0));
  EXPECT_EQ(string_view("-llib"), arguments_get(args, 1));
}

TEST_F(ModLdTest, TestUserArgsInvalid) {
  EXPECT_TRUE(arguments_insert(args, 0, "-g"));
  EXPECT_TRUE(arguments_insert(args, 1, CC_WRAPPER_USER_ARGS_END));
  EXPECT_TRUE(arguments_insert(args, 2, "-llib"));
  EXPECT_TRUE(arguments_insert(args, 3, CC_WRAPPER_USER_ARGS_BEGIN));
  EXPECT_TRUE(arguments_insert(args, 4, "-d"));

  struct ld_args ld_args;
  EXPECT_FALSE(ld_args_init(args, &ld_args));
}

TEST_F(ModLdTest, TestUserArgsTypical) {
  EXPECT_TRUE(arguments_insert(args, 0, "-g"));
  EXPECT_TRUE(arguments_insert(args, 1, CC_WRAPPER_USER_ARGS_BEGIN));
  EXPECT_TRUE(arguments_insert(args, 2, "-llib"));
  EXPECT_TRUE(arguments_insert(args, 3, CC_WRAPPER_USER_ARGS_END));
  EXPECT_TRUE(arguments_insert(args, 4, "-d"));

  struct ld_args ld_args;
  EXPECT_TRUE(ld_args_init(args, &ld_args));
  EXPECT_EQ(3u, arguments_nelems(args));
  EXPECT_EQ(1u, ld_args.user_args_start);
  EXPECT_EQ(2u, ld_args.user_args_end);
  EXPECT_EQ(string_view("-g"), string_view(arguments_get(args, 0)));
  EXPECT_EQ(string_view("-llib"), string_view(arguments_get(args, 1)));
  EXPECT_EQ(string_view("-d"), string_view(arguments_get(args, 2)));
}

TEST_F(ModLdTest, TestUserArgsInsert) {
  EXPECT_TRUE(arguments_insert(args, 0, "-g"));
  EXPECT_TRUE(arguments_insert(args, 1, CC_WRAPPER_USER_ARGS_BEGIN));
  EXPECT_TRUE(arguments_insert(args, 2, "-llib"));
  EXPECT_TRUE(arguments_insert(args, 3, "-llib2"));
  EXPECT_TRUE(arguments_insert(args, 4, CC_WRAPPER_USER_ARGS_END));
  EXPECT_TRUE(arguments_insert(args, 5, "-d"));

  struct ld_args ld_args;
  EXPECT_TRUE(ld_args_init(args, &ld_args));
  EXPECT_FALSE(ld_args_insert(&ld_args, 2, "-Oinvalid"));
  EXPECT_TRUE(ld_args_insert(&ld_args, 1, "-O1"));
  EXPECT_TRUE(ld_args_insert(&ld_args, 4, "-O2"));
  EXPECT_EQ(6u, arguments_nelems(args));
  EXPECT_EQ(2u, ld_args.user_args_start);
  EXPECT_EQ(4u, ld_args.user_args_end);
  EXPECT_EQ(string_view("-g"), string_view(arguments_get(args, 0)));
  EXPECT_EQ(string_view("-O1"), string_view(arguments_get(args, 1)));
  EXPECT_EQ(string_view("-llib"), string_view(arguments_get(args, 2)));
  EXPECT_EQ(string_view("-llib2"), string_view(arguments_get(args, 3)));
  EXPECT_EQ(string_view("-O2"), string_view(arguments_get(args, 4)));
  EXPECT_EQ(string_view("-d"), string_view(arguments_get(args, 5)));
}

TEST_F(ModLdTest, TestCheckLd) {
  SetExecType("cc");
  EXPECT_TRUE(mod_ld_rewrite(&GetExecInfo(), nullptr, nullptr));

  EXPECT_TRUE(arguments_insert(args, 0, "ld"));
  EXPECT_TRUE(arguments_insert(args, 1, CC_WRAPPER_USER_ARGS_BEGIN));
  EXPECT_TRUE(arguments_insert(args, 2, CC_WRAPPER_USER_ARGS_END));
  SetExecType("ld");
  EXPECT_TRUE(mod_ld_rewrite(&GetExecInfo(), args, nullptr));

  EXPECT_EQ(1u, arguments_nelems(args));
  EXPECT_EQ(string_view("ld"), string_view(arguments_get(args, 0)));
}

TEST_F(ModLdTest, TestNoDynamicLinker) {
  const char *no_dl = "/no-such-dl";

  SetExecType("ld");
  EXPECT_TRUE(arguments_insert(args, 0, "ld"));
  EXPECT_TRUE(arguments_insert(args, 1, "-dynamic-linker"));
  EXPECT_TRUE(arguments_insert(args, 2, no_dl));
  EXPECT_TRUE(arguments_insert(args, 3, "-dynamic-linker"));
  EXPECT_TRUE(arguments_insert(args, 4, no_dl));
  EXPECT_TRUE(arguments_insert(args, 5, CC_WRAPPER_USER_ARGS_BEGIN));
  EXPECT_TRUE(arguments_insert(args, 6, "-dynamic-linker"));
  EXPECT_TRUE(arguments_insert(args, 7, no_dl));
  EXPECT_TRUE(arguments_insert(args, 8, CC_WRAPPER_USER_ARGS_END));
  EXPECT_TRUE(arguments_insert(args, 9, "-dynamic-linker"));
  EXPECT_TRUE(arguments_insert(args, 10, no_dl));

  EXPECT_TRUE(mod_ld_rewrite(&GetExecInfo(), args, nullptr));

  EXPECT_EQ(9u, arguments_nelems(args));
  EXPECT_EQ(string_view("ld"), string_view(arguments_get(args, 0)));
  EXPECT_EQ(string_view("-dynamic-linker"),
            string_view(arguments_get(args, 1)));
  EXPECT_EQ(string_view(no_dl), string_view(arguments_get(args, 2)));
  EXPECT_EQ(string_view("-dynamic-linker"),
            string_view(arguments_get(args, 3)));
  EXPECT_EQ(string_view(no_dl), string_view(arguments_get(args, 4)));
  EXPECT_EQ(string_view("-dynamic-linker"),
            string_view(arguments_get(args, 5)));
  EXPECT_EQ(string_view(no_dl), string_view(arguments_get(args, 6)));
  EXPECT_EQ(string_view("-dynamic-linker"),
            string_view(arguments_get(args, 7)));
  EXPECT_EQ(string_view(no_dl), string_view(arguments_get(args, 8)));
}

TEST_F(ModLdTest, TestDynamicLinker) {
  const char *no_dl = "/no-such-dl";

  SetExecType("ld");
  EXPECT_TRUE(arguments_insert(args, 0, "ld"));
  EXPECT_TRUE(arguments_insert(args, 1, "-dynamic-linker"));
  EXPECT_TRUE(arguments_insert(args, 2, no_dl));
  EXPECT_TRUE(arguments_insert(args, 3, "-dynamic-linker"));
  EXPECT_TRUE(arguments_insert(args, 4, no_dl));
  EXPECT_TRUE(arguments_insert(args, 5, CC_WRAPPER_USER_ARGS_BEGIN));
  EXPECT_TRUE(arguments_insert(args, 6, "-dynamic-linker"));
  EXPECT_TRUE(arguments_insert(args, 7, no_dl));
  EXPECT_TRUE(arguments_insert(args, 8, CC_WRAPPER_USER_ARGS_END));
  EXPECT_TRUE(arguments_insert(args, 9, "-dynamic-linker"));
  EXPECT_TRUE(arguments_insert(args, 10, no_dl));

  target_dl = "/bad-dl";
  EXPECT_TRUE(mod_ld_rewrite(&GetExecInfo(), args, nullptr));

  EXPECT_EQ(9u, arguments_nelems(args));
  EXPECT_EQ(string_view("ld"), string_view(arguments_get(args, 0)));
  EXPECT_EQ(string_view("-dynamic-linker"),
            string_view(arguments_get(args, 1)));
  EXPECT_EQ(string_view(target_dl), string_view(arguments_get(args, 2)));
  EXPECT_EQ(string_view("-dynamic-linker"),
            string_view(arguments_get(args, 3)));
  EXPECT_EQ(string_view(target_dl), string_view(arguments_get(args, 4)));
  EXPECT_EQ(string_view("-dynamic-linker"),
            string_view(arguments_get(args, 5)));
  EXPECT_EQ(string_view(no_dl), string_view(arguments_get(args, 6)));
  EXPECT_EQ(string_view("-dynamic-linker"),
            string_view(arguments_get(args, 7)));
  EXPECT_EQ(string_view(no_dl), string_view(arguments_get(args, 8)));
}

TEST_F(ModLdTest, TestRpathAdding) {
  SetExecType("ld");
  EXPECT_TRUE(arguments_insert(args, 0, "ld"));
  EXPECT_TRUE(arguments_insert(args, 1, "-L/path1"));
  EXPECT_TRUE(arguments_insert(args, 2, CC_WRAPPER_USER_ARGS_BEGIN));
  EXPECT_TRUE(arguments_insert(args, 3, "-L/path2"));
  EXPECT_TRUE(arguments_insert(args, 4, CC_WRAPPER_USER_ARGS_END));
  EXPECT_TRUE(arguments_insert(args, 5, "-L/path3"));

  EXPECT_TRUE(mod_ld_rewrite(&GetExecInfo(), args, nullptr));

  EXPECT_EQ(8u, arguments_nelems(args));
  EXPECT_EQ(string_view("ld"), string_view(arguments_get(args, 0)));
  EXPECT_EQ(string_view("-L/path1"), string_view(arguments_get(args, 1)));
  EXPECT_EQ(string_view("-rpath"), string_view(arguments_get(args, 2)));
  EXPECT_EQ(string_view("/path1"), string_view(arguments_get(args, 3)));
  EXPECT_EQ(string_view("-L/path2"), string_view(arguments_get(args, 4)));
  EXPECT_EQ(string_view("-L/path3"), string_view(arguments_get(args, 5)));
  EXPECT_EQ(string_view("-rpath"), string_view(arguments_get(args, 6)));
  EXPECT_EQ(string_view("/path3"), string_view(arguments_get(args, 7)));
}

TEST_F(ModLdTest, TestLibcPathAdding) {
  SetExecType("ld");
  EXPECT_TRUE(arguments_insert(args, 0, "ld"));
  EXPECT_TRUE(arguments_insert(args, 1, "-d1"));
  EXPECT_TRUE(arguments_insert(args, 2, CC_WRAPPER_USER_ARGS_BEGIN));
  EXPECT_TRUE(arguments_insert(args, 3, "-d2"));
  EXPECT_TRUE(arguments_insert(args, 4, CC_WRAPPER_USER_ARGS_END));
  EXPECT_TRUE(arguments_insert(args, 5, "-d3"));

  target_libc_dynamic_libs = "/path-dynamic";
  target_libc_static_libs = "/path-static";
  EXPECT_TRUE(mod_ld_rewrite(&GetExecInfo(), args, nullptr));

  EXPECT_EQ(10u, arguments_nelems(args));
  EXPECT_EQ(string_view("ld"), string_view(arguments_get(args, 0)));
  EXPECT_EQ(string_view("-d1"), string_view(arguments_get(args, 1)));
  EXPECT_EQ(string_view("-d2"), string_view(arguments_get(args, 2)));
  EXPECT_EQ(string_view("-d3"), string_view(arguments_get(args, 3)));
  EXPECT_EQ(string("-L") + target_libc_dynamic_libs,
            string_view(arguments_get(args, 4)));
  EXPECT_EQ(string_view("-rpath"), string_view(arguments_get(args, 5)));
  EXPECT_EQ(string_view(target_libc_dynamic_libs),
            string_view(arguments_get(args, 6)));
  EXPECT_EQ(string("-L") + target_libc_static_libs,
            string_view(arguments_get(args, 7)));
  EXPECT_EQ(string_view("-rpath"), string_view(arguments_get(args, 8)));
  EXPECT_EQ(string_view(target_libc_static_libs),
            string_view(arguments_get(args, 9)));
}

TEST_F(ModLdTest, TestUserArgsOnly) {
  SetExecType("ld");
  EXPECT_TRUE(arguments_insert(args, 0, "ld"));
  EXPECT_TRUE(arguments_insert(args, 1, "-dynamic-linker"));
  EXPECT_TRUE(arguments_insert(args, 2, "/no-such-dl"));
  EXPECT_TRUE(arguments_insert(args, 3, "-L/no-such-path"));
  EXPECT_TRUE(arguments_insert(args, 4, "-d3"));

  EXPECT_TRUE(mod_ld_rewrite(&GetExecInfo(), args, nullptr));
  EXPECT_EQ(5u, arguments_nelems(args));
  EXPECT_EQ(string_view("ld"), string_view(arguments_get(args, 0)));
  EXPECT_EQ(string_view("-dynamic-linker"),
            string_view(arguments_get(args, 1)));
  EXPECT_EQ(string_view("/no-such-dl"), string_view(arguments_get(args, 2)));
  EXPECT_EQ(string_view("-L/no-such-path"),
            string_view(arguments_get(args, 3)));
  EXPECT_EQ(string_view("-d3"), string_view(arguments_get(args, 4)));
}

}  // namespace
