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
const char *target_libc_dynamic_libs;
const char *target_libc_static_libs;

namespace {

using std::string;
using std::string_view;

class ModCcTest : public ::ModTest {
 protected:
  void SetUp() override {
    ::ModTest::SetUp();

    target_libc_include = nullptr;
    target_libc_dynamic_libs = nullptr;
    target_libc_static_libs = nullptr;
  }
};

TEST_F(ModCcTest, TestCheckCc) {
  SetExecType("ld");
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), nullptr, nullptr));

  SetExecType("c");
  AppendArgs({"cc", "main.c"});
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  EXPECT_LT(2u, arguments_nelems(args));
}

TEST_F(ModCcTest, TestShortArgs) {
  SetExecType("c");
  AppendArgs({"cc"});
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  EXPECT_EQ(1u, arguments_nelems(args));
}

TEST_F(ModCcTest, TestNoLibc) {
  SetExecType("cpp");
  AppendArgs({"cc", "-I/usr/include", "main.c"});
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  ExpectArgs({
      "cc",
      "-Wl," CC_WRAPPER_USER_ARGS_BEGIN,
      "-I/usr/include",
      "main.c",
      "-Wl," CC_WRAPPER_USER_ARGS_END,
  });
}

TEST_F(ModCcTest, TestLibcNoLink) {
  target_libc_include = "/libc-include";
  target_libc_dynamic_libs = "/libc-dynamic-libs";
  target_libc_static_libs = "/libc-static-libs";
  SetExecType("c");
  AppendArgs({"cc", "-I/usr/include", "-c", "main.c"});
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  ExpectArgs({
      "cc",
      "-idirafter",
      target_libc_include,
      "-Wl," CC_WRAPPER_USER_ARGS_BEGIN,
      "-I/usr/include",
      "-c",
      "main.c",
      "-Wl," CC_WRAPPER_USER_ARGS_END,
  });
}

TEST_F(ModCcTest, TestLibcCpp) {
  target_libc_include = "/libc-include";
  target_libc_dynamic_libs = "/libc-dynamic-libs";
  target_libc_static_libs = "/libc-static-libs";
  SetExecType("cpp");
  AppendArgs({"cc", "-I/usr/include", "main.c"});
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  ExpectArgs({
      "cc",
      "-idirafter",
      target_libc_include,
      "-Wl," CC_WRAPPER_USER_ARGS_BEGIN,
      "-I/usr/include",
      "main.c",
      "-Wl," CC_WRAPPER_USER_ARGS_END,
  });
}

TEST_F(ModCcTest, TestLibcNostdinc) {
  target_libc_include = "/libc-include";
  SetExecType("cpp");
  AppendArgs({"cc", "-nostdinc", "-I/usr/include", "main.c"});
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  ExpectArgs({
      "cc",
      "-Wl," CC_WRAPPER_USER_ARGS_BEGIN,
      "-nostdinc",
      "-I/usr/include",
      "main.c",
      "-Wl," CC_WRAPPER_USER_ARGS_END,
  });
}

TEST_F(ModCcTest, TestLibcNoDefaultLibs) {
  target_libc_include = "/libc-include";
  target_libc_dynamic_libs = "/libc-dynamic-libs";
  target_libc_static_libs = "/libc-static-libs";
  SetExecType("c");

  AppendArgs({"cc", "-nodefaultlibs", "-I/usr/include", "main.c"});
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  ExpectArgs({
      "cc",
      "-idirafter",
      target_libc_include,
      "-Wl," CC_WRAPPER_USER_ARGS_BEGIN,
      "-nodefaultlibs",
      "-I/usr/include",
      "main.c",
      "-Wl," CC_WRAPPER_USER_ARGS_END,
  });
}

TEST_F(ModCcTest, TestLibcNoStdlib) {
  target_libc_include = "/libc-include";
  target_libc_dynamic_libs = "/libc-dynamic-libs";
  target_libc_static_libs = "/libc-static-libs";
  SetExecType("c");

  AppendArgs({"cc", "-nostdlib", "-I/usr/include", "main.c"});
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  ExpectArgs({
      "cc",
      "-idirafter",
      target_libc_include,
      "-Wl," CC_WRAPPER_USER_ARGS_BEGIN,
      "-nostdlib",
      "-I/usr/include",
      "main.c",
      "-Wl," CC_WRAPPER_USER_ARGS_END,
  });
}

TEST_F(ModCcTest, TestLibc) {
  target_libc_include = "/libc-include";
  target_libc_dynamic_libs = "/libc-dynamic-libs";
  target_libc_static_libs = "/libc-static-libs";
  SetExecType("c");
  AppendArgs({"cc", "-I/usr/include", "main.c"});
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  ExpectArgs({
      "cc",
      "-idirafter",
      target_libc_include,
      "-Wl," CC_WRAPPER_USER_ARGS_BEGIN,
      "-I/usr/include",
      "main.c",
      "-Wl," CC_WRAPPER_USER_ARGS_END,
      string("-L") + target_libc_dynamic_libs,
      string("-B") + target_libc_static_libs,
  });
}

TEST_F(ModCcTest, TestDebugNoRewrite) {
  SetExecType("c");
  AppendArgs({"cc", "-ggdb", "-g", "main.c"});
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  ExpectArgs({
      "cc",
      "-Wl," CC_WRAPPER_USER_ARGS_BEGIN,
      "-ggdb",
      "-g",
      "main.c",
      "-Wl," CC_WRAPPER_USER_ARGS_END,
  });
}

TEST_F(ModCcTest, TestDebugRewrite) {
  SetExecType("c");
  AppendArgs({"cc", "-ggdb", "-g", "main.c"});
  EXPECT_TRUE(environment_set(env, CC_WRAPPER_FLAG_REWRITE, "1"));
  EXPECT_TRUE(mod_cc_rewrite(&GetExecInfo(), args, env));

  ExpectArgs({
      "cc",
      "-Wl," CC_WRAPPER_USER_ARGS_BEGIN,
      "main.c",
      "-Wl," CC_WRAPPER_USER_ARGS_END,
  });
}

}  // namespace
