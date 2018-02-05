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

#include "environment.h"
#include "environment_test_util.h"
#include "string-util.h"

namespace {

using std::string_view;

TEST(EnvironmentTest, EnvironmentFreeNull) {
  environment_free(nullptr);
}

TEST(EnvironmentTest, EnvironmentEmpty) {
  const char *const env_empty[] = {
      nullptr,
  };

  struct environment *env = environment_from_array(env_empty);
  EXPECT_NE(nullptr, env);
  EXPECT_EQ(static_cast<size_t>(0), environment_nelems(env));
  environment_print(env, LOG_LEVEL_INFO);

  char **env_copy = environment_array_copy(env);
  EXPECT_EQ(nullptr, env_copy[0]);
  strings_free(env_copy);
  environment_free(env);
}

TEST(EnvironmentTest, EnvironmentConversion) {
  struct environment *env = environment_from_array(envp_sample);
  EXPECT_NE(nullptr, env);

  environment_print(env, LOG_LEVEL_INFO);
  EXPECT_EQ(static_cast<size_t>(3), environment_nelems(env));
  EXPECT_EQ(nullptr, environment_get(env, "NONEXISTANT"));
  EXPECT_EQ(string_view("hi"), string_view(environment_get(env, "VAR1")));
  EXPECT_EQ(string_view("lo"), string_view(environment_get(env, "VAR11")));
  EXPECT_EQ(string_view("off"), string_view(environment_get(env, "HI")));
  environment_free(env);
}

TEST(EnvironmentTest, EnvironmentConversionBad) {
  EXPECT_EQ(nullptr, environment_from_array(envp_sample_bad1));
  EXPECT_EQ(nullptr, environment_from_array(envp_sample_bad2));
}

TEST(EnvironmentTest, EnvironmentSet) {
  struct environment *env = environment_from_array(envp_sample);
  EXPECT_NE(nullptr, env);

  EXPECT_TRUE(environment_set(env, "VAR2", "on"));
  EXPECT_TRUE(environment_set(env, "VAR11", "hi"));
  EXPECT_TRUE(environment_set(env, "HI", nullptr));
  EXPECT_TRUE(environment_set(env, "NONEXISTANT", nullptr));

  environment_print(env, LOG_LEVEL_INFO);
  EXPECT_EQ(static_cast<size_t>(3), environment_nelems(env));
  EXPECT_EQ(nullptr, environment_get(env, "NONEXISTANT"));
  EXPECT_EQ(string_view("hi"), string_view(environment_get(env, "VAR1")));
  EXPECT_EQ(string_view("on"), string_view(environment_get(env, "VAR2")));
  EXPECT_EQ(string_view("hi"), string_view(environment_get(env, "VAR11")));
  EXPECT_EQ(nullptr, environment_get(env, "HI"));
  environment_free(env);
}

TEST(EnvironmentTest, EnvironmentCopy) {
  struct environment *env = environment_from_array(envp_sample);
  EXPECT_NE(nullptr, env);
  char **envp = environment_array_copy(env);
  EXPECT_NE(nullptr, envp);
  environment_free(env);

  EXPECT_EQ(string_view("HI=off"), string_view(envp[0]));
  EXPECT_EQ(string_view("VAR1=hi"), string_view(envp[1]));
  EXPECT_EQ(string_view("VAR11=lo"), string_view(envp[2]));
  EXPECT_EQ(nullptr, envp[3]);
  strings_free(envp);
}

}  // namespace
