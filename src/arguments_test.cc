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

#include "arguments.h"
#include "arguments_test_util.h"
#include "string-util.h"

namespace {

using std::string_view;

TEST(ArgumentsTest, ArgumentsFreeNull) {
  arguments_free(nullptr);
}

TEST(ArgumentsTest, ArgumentsEmpty) {
  const char *const argv_empty[] = {
      nullptr,
  };

  struct arguments *args = arguments_from_array(argv_empty);
  ASSERT_NE(nullptr, args);
  arguments_print(args, LOG_LEVEL_INFO);

  EXPECT_EQ(nullptr, arguments_array(args)[0]);
  arguments_free(args);
}

TEST(ArgumentsTest, Arguments) {
  struct arguments *args = arguments_from_array(argv_sample);
  ASSERT_NE(nullptr, args);
  arguments_print(args, LOG_LEVEL_INFO);

  const char *const *data = arguments_array(args);
  size_t i = 0;
  for (; argv_sample[i] != nullptr; ++i)
    EXPECT_EQ(string_view(argv_sample[i]), string_view(data[i]));
  EXPECT_EQ(nullptr, data[i]);
  arguments_free(args);
}

TEST(ArgumentTest, ArgumentsCopy) {
  struct arguments *args = arguments_from_array(argv_sample);
  ASSERT_NE(nullptr, args);
  char **argv_copy = arguments_array_copy(args);
  ASSERT_NE(nullptr, argv_copy);
  arguments_free(args);

  size_t i = 0;
  for (; argv_sample[i] != nullptr; ++i)
    EXPECT_EQ(string_view(argv_sample[i]), string_view(argv_copy[i]));
  EXPECT_EQ(nullptr, argv_copy[i]);
  strings_free(argv_copy);
}

}  // namespace
