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

  EXPECT_EQ(0u, ld_args.user_args_start);
  EXPECT_EQ(0u, ld_args.user_args_end);
  ExpectArgs({});
}

TEST_F(ModLdTest, TestUserArgsNoMarkers) {
  AppendArgs({"ld", "-g", "-llib"});

  struct ld_args ld_args;
  EXPECT_TRUE(ld_args_init(args, &ld_args));

  EXPECT_EQ(0u, ld_args.user_args_start);
  EXPECT_EQ(3u, ld_args.user_args_end);
  ExpectArgs({"ld", "-g", "-llib"});
}

TEST_F(ModLdTest, TestUserArgsNoEnd) {
  AppendArgs({"ld", "-g", CC_WRAPPER_USER_ARGS_BEGIN, "-llib"});

  struct ld_args ld_args;
  EXPECT_TRUE(ld_args_init(args, &ld_args));

  EXPECT_EQ(2u, ld_args.user_args_start);
  EXPECT_EQ(3u, ld_args.user_args_end);
  ExpectArgs({"ld", "-g", "-llib"});
}

TEST_F(ModLdTest, TestUserArgsNoBegin) {
  AppendArgs({"ld", "-g", CC_WRAPPER_USER_ARGS_END, "-llib"});

  struct ld_args ld_args;
  EXPECT_TRUE(ld_args_init(args, &ld_args));

  EXPECT_EQ(0u, ld_args.user_args_start);
  EXPECT_EQ(2u, ld_args.user_args_end);
  ExpectArgs({"ld", "-g", "-llib"});
}

TEST_F(ModLdTest, TestUserArgsInvalid) {
  AppendArgs({
      "ld",
      CC_WRAPPER_USER_ARGS_END,
      "-llib",
      CC_WRAPPER_USER_ARGS_BEGIN,
      "-d",
  });

  struct ld_args ld_args;
  EXPECT_FALSE(ld_args_init(args, &ld_args));
}

TEST_F(ModLdTest, TestUserArgsTypical) {
  AppendArgs({
      "ld",
      CC_WRAPPER_USER_ARGS_BEGIN,
      "-llib",
      CC_WRAPPER_USER_ARGS_END,
      "-d",
  });

  struct ld_args ld_args;
  EXPECT_TRUE(ld_args_init(args, &ld_args));

  EXPECT_EQ(1u, ld_args.user_args_start);
  EXPECT_EQ(2u, ld_args.user_args_end);
  ExpectArgs({"ld", "-llib", "-d"});
}

TEST_F(ModLdTest, TestUserArgsInsert) {
  AppendArgs({
      "ld",
      CC_WRAPPER_USER_ARGS_BEGIN,
      "-llib1",
      "-llib2",
      CC_WRAPPER_USER_ARGS_END,
      "-d",
  });

  struct ld_args ld_args;
  EXPECT_TRUE(ld_args_init(args, &ld_args));
  EXPECT_FALSE(ld_args_insert(&ld_args, 2, "-Oinvalid"));
  EXPECT_TRUE(ld_args_insert(&ld_args, 1, "-O1"));
  EXPECT_TRUE(ld_args_insert(&ld_args, 4, "-O2"));

  EXPECT_EQ(2u, ld_args.user_args_start);
  EXPECT_EQ(4u, ld_args.user_args_end);
  ExpectArgs({
      "ld",
      "-O1",
      "-llib1",
      "-llib2",
      "-O2",
      "-d",
  });
}

TEST_F(ModLdTest, TestCheckLd) {
  AppendArgs({
      "ld",
      CC_WRAPPER_USER_ARGS_BEGIN,
      CC_WRAPPER_USER_ARGS_END,
  });

  SetExecType("cc");
  EXPECT_TRUE(mod_ld_rewrite(&GetExecInfo(), nullptr, nullptr));

  SetExecType("ld");
  EXPECT_TRUE(mod_ld_rewrite(&GetExecInfo(), args, env));

  ExpectArgs({"ld"});
}

TEST_F(ModLdTest, TestNoDynamicLinker) {
  const char *no_dl = "/no-such-dl";

  SetExecType("ld");
  AppendArgs({
      "ld",
      "-dynamic-linker",
      no_dl,
      "-dynamic-linker",
      no_dl,
      CC_WRAPPER_USER_ARGS_BEGIN,
      "-dynamic-linker",
      no_dl,
      CC_WRAPPER_USER_ARGS_END,
      "-dynamic-linker",
      no_dl,
  });

  EXPECT_TRUE(mod_ld_rewrite(&GetExecInfo(), args, env));

  ExpectArgs({
      "ld",
      "-dynamic-linker",
      no_dl,
      "-dynamic-linker",
      no_dl,
      "-dynamic-linker",
      no_dl,
      "-dynamic-linker",
      no_dl,
  });
}

TEST_F(ModLdTest, TestDynamicLinker) {
  const char *no_dl = "/no-such-dl";

  SetExecType("ld");
  AppendArgs({
      "ld",
      "-dynamic-linker",
      no_dl,
      "-dynamic-linker",
      no_dl,
      CC_WRAPPER_USER_ARGS_BEGIN,
      "-dynamic-linker",
      no_dl,
      CC_WRAPPER_USER_ARGS_END,
      "-dynamic-linker",
      no_dl,
  });
  target_dl = "/bad-dl";

  EXPECT_TRUE(mod_ld_rewrite(&GetExecInfo(), args, env));

  ExpectArgs({
      "ld",
      "-dynamic-linker",
      target_dl,
      "-dynamic-linker",
      target_dl,
      "-dynamic-linker",
      no_dl,
      "-dynamic-linker",
      no_dl,
  });
}

TEST_F(ModLdTest, TestNoRpathAdding) {
  SetExecType("ld");
  AppendArgs({
      "ld",
      "-L/path1",
      CC_WRAPPER_USER_ARGS_BEGIN,
      "-L/path2",
      CC_WRAPPER_USER_ARGS_END,
      "-L/path3",
  });

  EXPECT_TRUE(mod_ld_rewrite(&GetExecInfo(), args, env));

  ExpectArgs({
      "ld",
      "-L/path1",
      "-rpath",
      "/path1",
      "-L/path2",
      "-L/path3",
      "-rpath",
      "/path3",
  });
}

TEST_F(ModLdTest, TestRpathAdding) {
  SetExecType("ld");
  AppendArgs({
      "ld",
      "-L/path1",
      CC_WRAPPER_USER_ARGS_BEGIN,
      "-L/path2",
      CC_WRAPPER_USER_ARGS_END,
      "-L/path3",
  });

  EXPECT_TRUE(mod_ld_rewrite(&GetExecInfo(), args, env));

  ExpectArgs({
      "ld",
      "-L/path1",
      "-rpath",
      "/path1",
      "-L/path2",
      "-L/path3",
      "-rpath",
      "/path3",
  });
}

TEST_F(ModLdTest, TestLibcPathAdding) {
  SetExecType("ld");
  AppendArgs({
      "ld",
      "-d1",
      CC_WRAPPER_USER_ARGS_BEGIN,
      "-d2",
      CC_WRAPPER_USER_ARGS_END,
      "-d3",
  });
  target_libc_dynamic_libs = "/path-dynamic";
  target_libc_static_libs = "/path-static";

  EXPECT_TRUE(mod_ld_rewrite(&GetExecInfo(), args, env));

  ExpectArgs({
      "ld",
      "-d1",
      "-d2",
      "-d3",
      string("-L") + target_libc_dynamic_libs,
      "-rpath",
      target_libc_dynamic_libs,
      string("-L") + target_libc_static_libs,
      "-rpath",
      target_libc_static_libs,
  });
}

TEST_F(ModLdTest, TestUserArgsOnly) {
  SetExecType("ld");
  AppendArgs({
      "ld",
      "-dynamic-linker",
      "/no-such-dl",
      "-L/no-such-path",
      "-d3",
  });

  EXPECT_TRUE(mod_ld_rewrite(&GetExecInfo(), args, env));

  ExpectArgs({
      "ld",
      "-dynamic-linker",
      "/no-such-dl",
      "-L/no-such-path",
      "-d3",
  });
}

}  // namespace
