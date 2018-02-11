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

#ifndef FUNC_H
#define FUNC_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Function should return negative for left of root,
 * 0 for equal to root, and positive for right of root
 */
typedef int (*binary_compare_t)(const void *root, const void *unplaced);

/*
 * Function for deallocating the data in a structure
 */
typedef void (*free_t)(void *data);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // FUNC_H
