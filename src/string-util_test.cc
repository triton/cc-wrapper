/*
 * Copyright 2017,2018 cc-wrapper authors
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
#include <cstdlib>
#include <string>
#include <string_view>

#include "array.h"
#include "string-util.h"

namespace {

using std::string;
using std::string_view;

TEST(StringUtilTest, StringCloneNull) {
  EXPECT_EQ(nullptr, string_clone(nullptr));
}

TEST(StringUtilTest, StringCloneValid) {
  const string_view expected = "this test str";
  char *clone = string_clone(expected.data());
  EXPECT_NE(expected.data(), clone);
  EXPECT_EQ(expected, string_view(clone));
  free(clone);
}

TEST(StringUtilTest, StringCloneNNull) {
  EXPECT_EQ(nullptr, string_clone_n(nullptr, 0));

  const size_t len = 10;
  char *str = string_clone_n(nullptr, len);
  EXPECT_EQ(string(len, '\0'), string_view(str, len));
  free(str);
}

TEST(StringUtilTest, StringCloneNValid) {
  const string_view expected = "This is a test string";
  const size_t len = expected.length() + 1;
  char *str = string_clone_n(expected.data(), len);
  EXPECT_EQ(expected, string_view(str));
  free(str);

  const size_t len2 = expected.length() + 10;
  string expected2(len2, '\0');
  memcpy(expected2.data(), expected.data(), expected.length());
  str = string_clone_n(expected.data(), len2);
  EXPECT_EQ(expected2, string_view(str, len2));
  free(str);
}

TEST(StringUtilTest, StringCloneNTrunc) {
  const string_view expected = "This is a test string";
  const size_t len = expected.length() - 3;
  char *str = string_clone_n(expected.data(), len);
  EXPECT_EQ(expected.substr(0, len), string_view(str, len));
  free(str);
}

TEST(StringUtilTest, StringArrayFreeNull) {
  string_array_free(nullptr);
}

TEST(StringUtilTest, StringArrayFree) {
  struct array *arr = array_init(sizeof(char *), 4);
  EXPECT_NE(nullptr, arr);
  auto arr_data = reinterpret_cast<char **>(array_data(arr));
  arr_data[0] = string_clone("example1");
  arr_data[1] = nullptr;
  arr_data[2] = string_clone("example2");
  arr_data[3] = nullptr;

  string_array_free(arr);
}

TEST(StringUtilTest, StringsFreeNull) {
  strings_free(nullptr);
}

TEST(StringUtilTest, StringsFree) {
  auto strings = reinterpret_cast<char **>(malloc(sizeof(char *) * 4));
  EXPECT_NE(nullptr, strings);
  strings[0] = string_clone("example1");
  strings[1] = string_clone("example2");
  strings[2] = string_clone("example3");
  strings[3] = nullptr;

  strings_free(strings);
}

}  // namespace
