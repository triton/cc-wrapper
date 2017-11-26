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

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "avl.h"
#include "func.h"

struct avl_node {
	void *data;
	bool data_owned;
	struct avl_node *left, *right;
	int8_t height;
};

struct avl {
	struct avl_node *root;
	binary_compare_t compare;
	free_t data_free;
};

struct avl *avl_init(binary_compare_t compare, free_t data_free)
{
	struct avl *avl;

	avl = malloc(sizeof(struct avl));
	if (avl == NULL)
		return NULL;

	avl->root = NULL;
	avl->compare = compare;
	avl->data_free = data_free;
	return avl;
}

void avl_node_free(struct avl_node *node, free_t data_free)
{
	if (node == NULL)
		return;

	if (node->data_owned)
		data_free(node->data);
	avl_node_free(node->left, data_free);
	avl_node_free(node->right, data_free);
	free(node);
}

void avl_free(struct avl *avl)
{
	if (avl == NULL)
		return;

	avl_node_free(avl->root, avl->data_free);
	free(avl);
}

int8_t avl_node_height(struct avl_node *root)
{
	if (root == NULL)
		return -1;

	return root->height;
}

void avl_node_height_fix(struct avl_node *root)
{
	int8_t left = avl_node_height(root->left);
	int8_t right = avl_node_height(root->right);
	root->height = (left > right ? left : right) + 1;
}

int8_t avl_height(struct avl *avl)
{
	return avl_node_height(avl->root);
}

/*
 * Populates the avl tree path to the element. Returns the index
 * in the path array of the element.
 */
int8_t avl_node_find(struct avl_node **path[], void *value,
		     binary_compare_t compare)
{
	int8_t path_idx = 0, compare_res;
	struct avl_node *node;

	for (; *path[path_idx] != NULL; ++path_idx) {
		node = *path[path_idx];
		compare_res = compare(node->data, value);
		if (compare_res == 0)
			return path_idx;
		path[path_idx + 1] =
		    compare_res < 0 ? &node->left : &node->right;
	}

	return path_idx;
}

/*
 * Works backwards up the path rebalancing and height fixing
 * the tree.
 */
void avl_path_fix(struct avl_node **path[], int8_t start)
{
	struct avl_node *node;
	int8_t delta;

	for (int8_t i = start; i >= 0; --i) {
		node = *path[i];

		/* Find imbalances and rebalance */
		delta =
		    avl_node_height(node->left) - avl_node_height(node->right);
		if (delta >= 2) {
			delta = avl_node_height(node->left->left) -
				avl_node_height(node->left->right);
			if (delta >= 1) {
				node->height = avl_node_height(node->right) + 1;

				*path[i] = node->left;
				node->left = (*path[i])->right;
				(*path[i])->right = node;
			} else {
				node->height = avl_node_height(node->right) + 1;
				node->left->height =
				    avl_node_height(node->left->left) + 1;

				*path[i] = node->left->right;
				node->left->right = (*path[i])->left;
				(*path[i])->left = node->left;
				node->left = (*path[i])->right;
				(*path[i])->right = node;
			}
		} else if (delta <= -2) {
			delta = avl_node_height(node->right->left) -
				avl_node_height(node->right->right);
			if (delta <= -1) {
				node->height = avl_node_height(node->left) + 1;

				*path[i] = node->right;
				node->right = (*path[i])->left;
				(*path[i])->left = node;
			} else {
				node->height = avl_node_height(node->left) + 1;
				node->right->height =
				    avl_node_height(node->right->right) + 1;

				*path[i] = node->right->left;
				node->right->left = (*path[i])->right;
				(*path[i])->right = node->right;
				node->right = (*path[i])->left;
				(*path[i])->left = node;
			}
		}

		/* Always recompute the height in case lower trees change */
		avl_node_height_fix(*path[i]);
	}
}

int avl_insert(struct avl *avl, void *value, bool own)
{
	struct avl_node **path[avl_height(avl) + 2];
	struct avl_node *node;
	int8_t found_idx;

	assert(!own || avl->data_free != NULL);

	/* Find where we should place our node in the tree */
	path[0] = &avl->root;
	found_idx = avl_node_find(path, value, avl->compare);
	if (*path[found_idx] != NULL)
		return EEXIST;

	/* Prepare and insert the new node */
	node = malloc(sizeof(struct avl_node));
	if (node == NULL)
		return ENOMEM;

	node->data = value;
	node->data_owned = own;
	node->left = NULL;
	node->right = NULL;
	node->height = 0;

	*path[found_idx] = node;

	/* Fixup our uptree path to make sure we maintain balance */
	avl_path_fix(path, found_idx - 1);

	return 0;
}

void *avl_find(struct avl *avl, void *value)
{
	struct avl_node **path[avl_height(avl) + 2];
	int8_t found_idx;

	path[0] = &avl->root;
	found_idx = avl_node_find(path, value, avl->compare);
	if (*path[found_idx] == NULL)
		return NULL;

	return (*path[found_idx])->data;
}

void *avl_remove(struct avl *avl, void *value)
{
	struct avl_node **path[avl_height(avl) + 2];
	struct avl_node *deleted_node;
	int8_t found_idx, replacement_idx;
	void *data;

	/* Find the path to the node through the tree */
	path[0] = &avl->root;
	found_idx = avl_node_find(path, value, avl->compare);
	deleted_node = *path[found_idx];
	if (deleted_node == NULL)
		return NULL;

	/* Identify and replace the node */
	if (deleted_node->left == NULL) {
		/* There is no node to our left so this is a trivial
		 * replacement */
		*path[found_idx] = deleted_node->right;
		replacement_idx = found_idx;
	} else {
		/* We need to find the in-order predecessor to use a replacement
		 * since we might have a left and right child. */
		replacement_idx = found_idx + 1;
		path[replacement_idx] = &deleted_node->left;
		for (; (*path[replacement_idx])->right != NULL;
		     ++replacement_idx)
			path[replacement_idx + 1] =
			    &(*path[replacement_idx])->right;

		/* Replace the node in the tree with the predecessor */
		*path[found_idx] = *path[replacement_idx];
		/* Since this is an in-order predecessor it can't have a ->right
		 * child this makes the replacement of the node trivial by
		 * moving in its left child*/
		*path[replacement_idx] = (*path[replacement_idx])->left;
		/* We originally stored a reference to the deleted node's left
		 * child in our path stack. Fix this reference to relate to the
		 * new replacement */
		path[found_idx + 1] = &(*path[found_idx])->left;
		/* Trivially adopt the children of the deleted node */
		(*path[found_idx])->left = deleted_node->left;
		(*path[found_idx])->right = deleted_node->right;
	}

	/* Delete our old node, saving the data */
	data = deleted_node->data;
	free(deleted_node);

	/* Fixup our uptree path to make sure we maintain balance */
	avl_path_fix(path, replacement_idx - 1);

	return data;
}
