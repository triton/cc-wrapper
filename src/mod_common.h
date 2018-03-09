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

#ifndef MOD_COMMON_H
#define MOD_COMMON_H

#include <stdbool.h>

#include "arguments.h"
#include "environment.h"
#include "execinfo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CC_WRAPPER_USER_ARGS_BEGIN "--cc-wrapper-begin"
#define CC_WRAPPER_USER_ARGS_END "--cc-wrapper-end"

#define CC_WRAPPER_FLAG_REWRITE "CC_WRAPPER_FLAG_REWRITE"
bool should_rewrite_flags(const struct environment *env);

bool mod_common_rewrite(const struct exec_info *exec_info,
			struct arguments *args, struct environment *env);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // MOD_COMMON_H
