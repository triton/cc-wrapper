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

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <stdbool.h>
#include <sys/types.h>

#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

struct environment;

/*
 * Creates a new environment struct from the c-style envp list
 * Returns NULL on failure.
 */
struct environment *environment_from_array(const char *const envp[]);

/*
 * Frees the environment struct
 */
void environment_free(struct environment *env);

/*
 * Returns the number of environment variables
 */
size_t environment_nelems(const struct environment *env);

/*
 * Prints out the environment using the built in logging facility
 */
void environment_print(const struct environment *env, enum log_level log_level);

/*
 * Returns a reference to the value for a specified environment variable
 * Returns NULL if the variable doesn't exist
 */
const char *environment_get(const struct environment *env, const char *key);

/*
 * Sets an environment variable to a new value
 * If val is NULL the element is removed from the environment.
 */
bool environment_set(struct environment *env, const char *key, const char *val);

/*
 * Returns a mutable copy of the arguments list as a c-style string.
 * The list is allocated on the heap and should be freed by the callee
 * using strings_free().
 */
char **environment_array_copy(const struct environment *env);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // ENVIRONMENT_H
