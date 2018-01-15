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

#include "log.h"

namespace {

using std::string_view;

TEST(LogTest, LogLevelFromString) {
  EXPECT_EQ(LOG_LEVEL_NONE, log_level_from_string("NONE"));
  EXPECT_EQ(LOG_LEVEL_NONE, log_level_from_string("none"));

  EXPECT_EQ(LOG_LEVEL_FATAL, log_level_from_string("FATAL"));
  EXPECT_EQ(LOG_LEVEL_FATAL, log_level_from_string("fatal"));

  EXPECT_EQ(LOG_LEVEL_ERROR, log_level_from_string("ERROR"));
  EXPECT_EQ(LOG_LEVEL_ERROR, log_level_from_string("error"));

  EXPECT_EQ(LOG_LEVEL_WARN, log_level_from_string("WARN"));
  EXPECT_EQ(LOG_LEVEL_WARN, log_level_from_string("warn"));

  EXPECT_EQ(LOG_LEVEL_INFO, log_level_from_string("INFO"));
  EXPECT_EQ(LOG_LEVEL_INFO, log_level_from_string("info"));

  EXPECT_EQ(LOG_LEVEL_DEBUG, log_level_from_string("DEBUG"));
  EXPECT_EQ(LOG_LEVEL_DEBUG, log_level_from_string("debug"));

  EXPECT_EQ(LOG_LEVEL_TRACE, log_level_from_string("TRACE"));
  EXPECT_EQ(LOG_LEVEL_TRACE, log_level_from_string("trace"));

  EXPECT_EQ(LOG_LEVEL_UNKNOWN, log_level_from_string("NON"));
  EXPECT_EQ(LOG_LEVEL_UNKNOWN, log_level_from_string("non"));
  EXPECT_EQ(LOG_LEVEL_UNKNOWN, log_level_from_string("NONEE"));
  EXPECT_EQ(LOG_LEVEL_UNKNOWN, log_level_from_string("nonee"));

  EXPECT_EQ(LOG_LEVEL_UNKNOWN, log_level_from_string("1"));
}

TEST(LogTest, LogLevelToString) {
  EXPECT_EQ(string_view("NONE"),
            string_view(log_level_to_string(LOG_LEVEL_NONE)));
  EXPECT_EQ(string_view("FATAL"),
            string_view(log_level_to_string(LOG_LEVEL_FATAL)));
  EXPECT_EQ(string_view("ERROR"),
            string_view(log_level_to_string(LOG_LEVEL_ERROR)));
  EXPECT_EQ(string_view("WARN"),
            string_view(log_level_to_string(LOG_LEVEL_WARN)));
  EXPECT_EQ(string_view("INFO"),
            string_view(log_level_to_string(LOG_LEVEL_INFO)));
  EXPECT_EQ(string_view("DEBUG"),
            string_view(log_level_to_string(LOG_LEVEL_DEBUG)));
  EXPECT_EQ(string_view("TRACE"),
            string_view(log_level_to_string(LOG_LEVEL_TRACE)));
}

TEST(LogTest, FatalMessages) {
  log_level = LOG_LEVEL_FATAL;
  EXPECT_EXIT(LOG_FATAL("hi"), testing::ExitedWithCode(1), "^hi$");
  log_level = LOG_LEVEL_NONE;
  EXPECT_EXIT(LOG_FATAL("hi"), testing::ExitedWithCode(1), "^$");
}

TEST(LogTest, PrintLevels) {
  log_level = LOG_LEVEL_INFO;
  EXPECT_EXIT(
      {
        LOG_ERROR("hi%s\n", "hi");
        exit(0);
      },
      testing::ExitedWithCode(0), "^hihi\n$");
  EXPECT_EXIT(
      {
        LOG_INFO("hi");
        exit(0);
      },
      testing::ExitedWithCode(0), "^hi$");
  EXPECT_EXIT(
      {
        LOG_DEBUG("hi");
        exit(0);
      },
      testing::ExitedWithCode(0), "^$");
}

}  // namespace
