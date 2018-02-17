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

  EXPECT_EQ(0u, arguments_nelems(args));
  EXPECT_EQ(nullptr, arguments_array(args)[0]);
  arguments_free(args);
}

TEST(ArgumentsTest, Arguments) {
  struct arguments *args = arguments_from_array(argv_sample);
  ASSERT_NE(nullptr, args);
  arguments_print(args, LOG_LEVEL_INFO);

  EXPECT_EQ(3u, arguments_nelems(args));
  const char *const *data = arguments_array(args);
  size_t i = 0;
  for (; argv_sample[i] != nullptr; ++i)
    EXPECT_EQ(string_view(argv_sample[i]), string_view(data[i]));
  EXPECT_EQ(nullptr, data[i]);
  arguments_free(args);
}

TEST(ArgumentsTest, ArgumentsInsert) {
  const string_view i0 = "boop0";
  const string_view i2 = "boop2";
  const string_view i5 = "boop5";

  struct arguments *args = arguments_from_array(argv_sample);
  ASSERT_NE(nullptr, args);
  EXPECT_FALSE(arguments_insert(args, 1, nullptr));
  EXPECT_FALSE(arguments_insert(args, 4, "boop"));
  EXPECT_TRUE(arguments_insert(args, 0, i0.data()));
  EXPECT_TRUE(arguments_insert(args, 2, i2.data()));
  EXPECT_TRUE(arguments_insert(args, 5, i5.data()));
  EXPECT_FALSE(arguments_insert(args, 7, "boop"));

  EXPECT_EQ(6u, arguments_nelems(args));
  const char *const *data = arguments_array(args);
  EXPECT_EQ(i0, string_view(data[0]));
  EXPECT_EQ(string_view(argv_sample[0]), string_view(data[1]));
  EXPECT_EQ(i2, string_view(data[2]));
  EXPECT_EQ(string_view(argv_sample[1]), string_view(data[3]));
  EXPECT_EQ(string_view(argv_sample[2]), string_view(data[4]));
  EXPECT_EQ(i5, string_view(data[5]));
  EXPECT_EQ(nullptr, data[6]);
  arguments_free(args);
}

TEST(ArgumentsTest, ArgumentsSet) {
  const string_view i1 = "boop1";

  struct arguments *args = arguments_from_array(argv_sample);
  ASSERT_NE(nullptr, args);
  EXPECT_FALSE(arguments_set(args, 1, nullptr));
  EXPECT_FALSE(arguments_set(args, 3, "boop"));
  EXPECT_TRUE(arguments_set(args, 1, i1.data()));

  EXPECT_EQ(3u, arguments_nelems(args));
  const char *const *data = arguments_array(args);
  EXPECT_EQ(string_view(argv_sample[0]), string_view(data[0]));
  EXPECT_EQ(i1, string_view(data[1]));
  EXPECT_EQ(string_view(argv_sample[2]), string_view(data[2]));
  arguments_free(args);
}

TEST(ArgumentsTest, ArgumentsRemove) {
  struct arguments *args = arguments_from_array(argv_sample);
  ASSERT_NE(nullptr, args);
  EXPECT_FALSE(arguments_remove(args, 3));
  EXPECT_TRUE(arguments_remove(args, 1));

  EXPECT_EQ(2u, arguments_nelems(args));
  const char *const *data = arguments_array(args);
  EXPECT_EQ(string_view(argv_sample[0]), string_view(data[0]));
  EXPECT_EQ(string_view(argv_sample[2]), string_view(data[1]));
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
