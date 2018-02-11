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

#ifndef AVL_H
#define AVL_H

#include <stdbool.h>
#include <stdint.h>

#include "func.h"

#ifdef __cplusplus
extern "C" {
#endif

struct avl;

/*
 * Returns a new AVL tree or NULL on failure. data_free can be NULL
 * if you don't need the AVL to ever own data.
 */
struct avl *avl_init(binary_compare_t compare, free_t data_free);

/*
 * Frees the AVL and all of the owned data elements.
 */
void avl_free(struct avl *avl);

/*
 * Returns the height of the current AVL tree
 */
int8_t avl_height(const struct avl *avl);

/*
 * Inserts a value into the AVL. Fails if the value is already contained
 * in the tree. Returns 0 on success, ENOMEM on failure to allocate a node,
 * EEXIST if a node already exists.
 *
 * The AVL can automatically free this node if own is true.
 */
int avl_insert(struct avl *avl, void *value, bool own);

/*
 * Returns the data stored in the AVL for value
 */
void *avl_find(const struct avl *avl, const void *value);

/*
 * Removes a node in the AVL by value. Returns the data of the node
 * being deleted, NULL if no node existed.
 *
 * Note: This data is not freed by the AVL even if owned.
 */
void *avl_remove(struct avl *avl, const void *value);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // AVL_H
