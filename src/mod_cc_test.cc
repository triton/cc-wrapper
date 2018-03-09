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
#include "mod_cc.h"
#include "mod_common.h"
#include "mod_test_util.hh"
#include "string-util.h"

const char *target_libc_include;
const char *target_libc_static_libs;

namespace {

using std::string;
using std::string_view;

class ModCcTest : public ::ModTest {
 protected:
  void SetUp() override {
    ::ModTest::SetUp();

    target_libc_include = nullptr;
    target_libc_static_libs = nullptr;
  }
};

TEST_F(ModCcTest, TestCheckCc) {
  SetExecType("ld");
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), nullptr, nullptr));

  SetExecType("c");
  EXPECT_TRUE(arguments_insert(args, 0, "cc"));
  EXPECT_TRUE(arguments_insert(args, 1, "main.c"));
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  EXPECT_LT(2u, arguments_nelems(args));
}

TEST_F(ModCcTest, TestShortArgs) {
  SetExecType("c");
  EXPECT_TRUE(arguments_insert(args, 0, "cc"));
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  EXPECT_EQ(1u, arguments_nelems(args));
}

TEST_F(ModCcTest, TestNoLibc) {
  SetExecType("cpp");
  EXPECT_TRUE(arguments_insert(args, 0, "cc"));
  EXPECT_TRUE(arguments_insert(args, 1, "-I/usr/include"));
  EXPECT_TRUE(arguments_insert(args, 2, "main.c"));
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  EXPECT_EQ(5u, arguments_nelems(args));
  EXPECT_EQ(string_view("cc"), string_view(arguments_get(args, 0)));
  EXPECT_EQ(string_view("-Wl," CC_WRAPPER_USER_ARGS_BEGIN),
            string_view(arguments_get(args, 1)));
  EXPECT_EQ(string_view("-I/usr/include"), string_view(arguments_get(args, 2)));
  EXPECT_EQ(string_view("main.c"), string_view(arguments_get(args, 3)));
  EXPECT_EQ(string_view("-Wl," CC_WRAPPER_USER_ARGS_END),
            string_view(arguments_get(args, 4)));
}

TEST_F(ModCcTest, TestLibcNoLink) {
  SetExecType("cpp");
  EXPECT_TRUE(arguments_insert(args, 0, "cc"));
  EXPECT_TRUE(arguments_insert(args, 1, "-I/usr/include"));
  EXPECT_TRUE(arguments_insert(args, 2, "-c"));
  EXPECT_TRUE(arguments_insert(args, 3, "main.c"));
  target_libc_include = "/libc-include";
  target_libc_static_libs = "/libc-libs";
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  EXPECT_EQ(8u, arguments_nelems(args));
  EXPECT_EQ(string_view("cc"), string_view(arguments_get(args, 0)));
  EXPECT_EQ(string_view("-idirafter"), string_view(arguments_get(args, 1)));
  EXPECT_EQ(string_view(target_libc_include),
            string_view(arguments_get(args, 2)));
  EXPECT_EQ(string_view("-Wl," CC_WRAPPER_USER_ARGS_BEGIN),
            string_view(arguments_get(args, 3)));
  EXPECT_EQ(string_view("-I/usr/include"), string_view(arguments_get(args, 4)));
  EXPECT_EQ(string_view("-c"), string_view(arguments_get(args, 5)));
  EXPECT_EQ(string_view("main.c"), string_view(arguments_get(args, 6)));
  EXPECT_EQ(string_view("-Wl," CC_WRAPPER_USER_ARGS_END),
            string_view(arguments_get(args, 7)));
}

TEST_F(ModCcTest, TestLibcNostdinc) {
  SetExecType("cpp");
  EXPECT_TRUE(arguments_insert(args, 0, "cc"));
  EXPECT_TRUE(arguments_insert(args, 1, "-nostdinc"));
  EXPECT_TRUE(arguments_insert(args, 2, "-I/usr/include"));
  EXPECT_TRUE(arguments_insert(args, 3, "main.c"));
  target_libc_include = "/libc-include";
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  EXPECT_EQ(6u, arguments_nelems(args));
  EXPECT_EQ(string_view("cc"), string_view(arguments_get(args, 0)));
  EXPECT_EQ(string_view("-Wl," CC_WRAPPER_USER_ARGS_BEGIN),
            string_view(arguments_get(args, 1)));
  EXPECT_EQ(string_view("-nostdinc"), string_view(arguments_get(args, 2)));
  EXPECT_EQ(string_view("-I/usr/include"), string_view(arguments_get(args, 3)));
  EXPECT_EQ(string_view("main.c"), string_view(arguments_get(args, 4)));
  EXPECT_EQ(string_view("-Wl," CC_WRAPPER_USER_ARGS_END),
            string_view(arguments_get(args, 5)));
}

TEST_F(ModCcTest, TestLibc) {
  SetExecType("cpp");
  EXPECT_TRUE(arguments_insert(args, 0, "cc"));
  EXPECT_TRUE(arguments_insert(args, 1, "-I/usr/include"));
  EXPECT_TRUE(arguments_insert(args, 2, "main.c"));
  target_libc_include = "/libc-include";
  target_libc_static_libs = "/libc-libs";
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  EXPECT_EQ(8u, arguments_nelems(args));
  EXPECT_EQ(string_view("cc"), string_view(arguments_get(args, 0)));
  EXPECT_EQ(string_view("-idirafter"), string_view(arguments_get(args, 1)));
  EXPECT_EQ(string_view(target_libc_include),
            string_view(arguments_get(args, 2)));
  EXPECT_EQ(string_view("-Wl," CC_WRAPPER_USER_ARGS_BEGIN),
            string_view(arguments_get(args, 3)));
  EXPECT_EQ(string_view("-I/usr/include"), string_view(arguments_get(args, 4)));
  EXPECT_EQ(string_view("main.c"), string_view(arguments_get(args, 5)));
  EXPECT_EQ(string_view("-Wl," CC_WRAPPER_USER_ARGS_END),
            string_view(arguments_get(args, 6)));
  EXPECT_EQ(string("-B") + target_libc_static_libs,
            string_view(arguments_get(args, 7)));
}

TEST_F(ModCcTest, TestDebugNoRewrite) {
  SetExecType("c");
  EXPECT_TRUE(arguments_insert(args, 0, "cc"));
  EXPECT_TRUE(arguments_insert(args, 1, "-ggdb"));
  EXPECT_TRUE(arguments_insert(args, 2, "-g"));
  EXPECT_TRUE(arguments_insert(args, 3, "main.c"));
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  EXPECT_EQ(6u, arguments_nelems(args));
  EXPECT_EQ(string_view("cc"), string_view(arguments_get(args, 0)));
  EXPECT_EQ(string_view("-Wl," CC_WRAPPER_USER_ARGS_BEGIN),
            string_view(arguments_get(args, 1)));
  EXPECT_EQ(string_view("-ggdb"), string_view(arguments_get(args, 2)));
  EXPECT_EQ(string_view("-g"), string_view(arguments_get(args, 3)));
  EXPECT_EQ(string_view("main.c"), string_view(arguments_get(args, 4)));
  EXPECT_EQ(string_view("-Wl," CC_WRAPPER_USER_ARGS_END),
            string_view(arguments_get(args, 5)));
}

TEST_F(ModCcTest, TestDebugRewrite) {
  SetExecType("c");
  EXPECT_TRUE(arguments_insert(args, 0, "cc"));
  EXPECT_TRUE(arguments_insert(args, 1, "-ggdb"));
  EXPECT_TRUE(arguments_insert(args, 2, "-g"));
  EXPECT_TRUE(arguments_insert(args, 3, "main.c"));
  EXPECT_TRUE(environment_set(env, "CC_WRAPPER_FLAG_REWRITE", "1"));
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  EXPECT_EQ(4u, arguments_nelems(args));
  EXPECT_EQ(string_view("cc"), string_view(arguments_get(args, 0)));
  EXPECT_EQ(string_view("-Wl," CC_WRAPPER_USER_ARGS_BEGIN),
            string_view(arguments_get(args, 1)));
  EXPECT_EQ(string_view("main.c"), string_view(arguments_get(args, 2)));
  EXPECT_EQ(string_view("-Wl," CC_WRAPPER_USER_ARGS_END),
            string_view(arguments_get(args, 3)));
}

}  // namespace
