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

#include "environment.h"

static const char *const empty_arr[] = {nullptr};

class ModTest : public ::testing::Test {
 protected:
  struct environment *env;

  void SetUp() override {
  	::testing::Test::SetUp();

    env = environment_from_array(empty_arr);
  }

  void TearDown() override {
  	::testing::Test::TearDown();

    environment_free(env);
  }
};
