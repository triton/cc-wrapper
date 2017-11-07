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

#ifndef ARRAY_H
#define ARRAY_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct array;

/*
 * Returns a new array with the number of elems specified by elems or 
 * NULL on failure.
 */
struct array *array_init(size_t elem_size, size_t nelems);

/*
 * Frees the array
 */
void array_free(struct array *array);

/*
 * Returns whether or not the operation was successful.
 * If the operation fails the array remains the same as it was.
 */
bool array_resize(struct array *array, size_t nelems);

/*
 * Returns the number of elements available in the array.
 */
size_t array_nelems(struct array *array);

/*
 * Returns the array data backing the structure.
 * This can be accessed like any regular T[].
 * Note this is invalidated by an array_resize returning true.
 */
void *array_data(struct array *array);

/*
 * Returns the size of the underlying array allocation
 */
size_t array_allocation(struct array *array);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // ARRAY_H
