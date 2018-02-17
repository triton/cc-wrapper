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

#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stdbool.h>

#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

struct arguments;

/*
 * Creates a new arguments struct from the c-style arguments list
 * Returns NULL on failure.
 */
struct arguments *arguments_from_array(const char *const argv[]);

/*
 * Frees the arguments struct.
 */
void arguments_free(struct arguments *args);

/*
 * Prints out the arguments using the built in logging facility.
 */
void arguments_print(const struct arguments *args, enum log_level log_level);

/*
 * Gets the number of arguments in the list
 */
size_t arguments_nelems(const struct arguments *args);

/*
 * Copies the specified value and stores it in the arguments list at idx.
 * Pushed the rest of the elements up the list.
 * Returns true on success, false if allocation fails or the element is out
 * of bounds.
 */
bool arguments_insert(struct arguments *args, size_t idx, const char *arg);

/*
 * Copies the specified value and overwrites the argument at idx.
 * Returns true on success, false if allocation fails or idx is out of bounds.
 */
bool arguments_set(struct arguments *args, size_t idx, const char *arg);

/*
 * Removes the argument at idx and shrinks args list down.
 * Returns true on success, false if idx is out of bounds.
 */
bool arguments_remove(struct arguments *args, size_t idx);

/*
 * Returns a reference to the c-style arguments list
 */
const char *const *arguments_array(const struct arguments *args);

/*
 * Returns a mutable copy of the arguments list as a c-style string.
 * This list is allocated on the heap and should be freed by the callee
 * using strings_free().
 */
char **arguments_array_copy(const struct arguments *args);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // ARGUMENTS_H
