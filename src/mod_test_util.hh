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

#pragma once

#include <gtest/gtest.h>
#include <string_view>

#include "arguments.h"
#include "environment.h"
#include "execinfo.h"

static const char *const empty_arr[] = {nullptr};

class ModTest : public ::testing::Test {
 protected:
  struct arguments *args;
  struct environment *env;

  void SetUp() override {
  	::testing::Test::SetUp();

    args = arguments_from_array(empty_arr);
    env = environment_from_array(empty_arr);

    exec_info.name = nullptr;
    exec_info.path = nullptr;
    exec_info.package = nullptr;
    exec_info.type = nullptr;
    exec_info.prefer = 0;
  }

  void TearDown() override {
  	::testing::Test::TearDown();

    arguments_free(args);
    environment_free(env);
    delete[] exec_info.type;
  }

  void SetExecType(std::string_view str) {
    delete[] exec_info.type;
    exec_info.type = new char[str.size() + 1];
    memcpy(exec_info.type, str.data(), str.size());
    exec_info.type[str.size()] = '\0';
  }

  const struct exec_info &GetExecInfo() {
    return exec_info;
  }

 private:
  struct exec_info exec_info;
};
