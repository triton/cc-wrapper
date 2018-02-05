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

#include <stddef.h>

#include "environment_test_util.h"

const char *const envp_sample[] = {
    "VAR1=hi",
    "VAR11=lo",
    "HI=off",
    NULL,
};

const char *const envp_sample_bad1[] = {
    "VAR1hi",
    NULL,
};

const char *const envp_sample_bad2[] = {
    "VAR1=hi",
    "VAR1=lo",
    "HI=off",
    NULL,
};
