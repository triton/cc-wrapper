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

#include <string.h>
#include <sys/types.h>

#include "avl.h"
#include "environment.h"
#include "log.h"
#include "string-util.h"

struct env_node {
	char *key;
	char *val;
};

struct environment {
	struct avl *dict;
};

int env_node_compare(const void *root, const void *unplaced)
{
	const struct env_node *root_node = root, *unplaced_node = unplaced;
	return strcmp(unplaced_node->key, root_node->key);
}

void env_node_free(void *data)
{
	struct env_node *node = data;
	if (node == NULL)
		return;

	free(node->key);
	free(node->val);
	free(node);
}

struct environment *environment_from_array(const char *const envp[])
{
	struct env_node *node = NULL;
	struct environment *env = malloc(sizeof(struct environment));
	if (env == NULL)
		goto error;

	env->dict = avl_init(env_node_compare, env_node_free);
	if (env->dict == NULL)
		goto error;

	size_t i = 0;
	for (; envp[i] != NULL; ++i) {
		node = malloc(sizeof(struct env_node));
		if (node == NULL)
			goto error;
		node->key = NULL;
		node->val = NULL;

		const char *val = strchr(envp[i], '=');
		if (val == NULL)
			goto error;

		node->key = string_clone_n(envp[i], 1 + val - envp[i]);
		if (node->key == NULL)
			goto error;
		node->key[val - envp[i]] = '\0';

		node->val = string_clone(val + 1);
		if (node->val == NULL)
			goto error;

		if (avl_insert(env->dict, node, true) != 0)
			goto error;
		node = NULL;
	}

	return env;
error:
	env_node_free(node);
	environment_free(env);
	return NULL;
}

void environment_free(struct environment *env)
{
	if (env == NULL)
		return;

	if (env->dict != NULL)
		avl_free(env->dict);
	free(env);
}

size_t environment_nelems(const struct environment *env)
{
	return avl_nelems(env->dict);
}

struct print_data {
	enum log_level log_level;
};

void environment_print_cb(const void *data, void *priv)
{
	const struct env_node *node = data;
	const struct print_data *print_data = priv;
	log_printf(print_data->log_level, "  %s=%s\n", node->key, node->val);
}

void environment_print(const struct environment *env, enum log_level log_level)
{
	struct print_data print_data = {
	    .log_level = log_level,
	};
	avl_order_traverse(env->dict, environment_print_cb, &print_data);
	log_printf(log_level, "\n");
}

const char *environment_get(const struct environment *env, const char *key)
{
	const struct env_node test_node = {
	    .key = (char *)key,
	};
	const struct env_node *node = avl_find(env->dict, &test_node);
	if (node == NULL)
		return NULL;
	return node->val;
}

bool environment_set_new(struct environment *env, const char *key,
			 const char *val)
{
	struct env_node *node = malloc(sizeof(struct env_node));
	if (node == NULL)
		goto error;
	node->key = NULL;
	node->val = NULL;

	node->key = string_clone(key);
	if (node->key == NULL)
		goto error;
	node->val = string_clone(val);
	if (node->val == NULL)
		goto error;

	if (avl_insert(env->dict, node, true) != 0)
		goto error;

	return true;
error:
	env_node_free(node);
	return false;
}

bool environment_set(struct environment *env, const char *key, const char *val)
{
	const struct env_node test_node = {
	    .key = (char *)key,
	};

	/* Handle the delete case */
	if (val == NULL) {
		env_node_free(avl_remove(env->dict, &test_node));
		return true;
	}

	struct env_node *node = avl_find(env->dict, &test_node);
	if (node == NULL)
		return environment_set_new(env, key, val);

	char *new_val = string_clone(val);
	if (new_val == NULL)
		return false;

	free(node->val);
	node->val = new_val;
	return true;
}

struct environment_array_copy {
	char **arr;
};

void environment_array_copy_cb(const void *data, void *priv)
{
	const struct env_node *node = data;
	struct environment_array_copy *copy = priv;

	if (copy->arr == NULL)
		return;

	const size_t key_len = strlen(node->key);
	const size_t val_len = strlen(node->val);
	const size_t res_size = key_len + 1 + val_len + 1;
	char *res = malloc(sizeof(char) * res_size);
	if (res == NULL) {
		strings_free(copy->arr);
		copy->arr = NULL;
		return;
	}

	memcpy(res, node->key, key_len);
	res[key_len] = '=';
	memcpy(res + key_len + 1, node->val, val_len);
	res[res_size - 1] = '\0';
	*(copy->arr++) = res;
}

char **environment_array_copy(const struct environment *env)
{
	const size_t arr_size = avl_nelems(env->dict) + 1;
	struct environment_array_copy copy;
	char **ret = copy.arr = malloc(sizeof(char *) * arr_size);
	if (ret == NULL)
		goto error;

	for (size_t i = 0; i < arr_size; ++i)
		ret[i] = NULL;

	avl_order_traverse(env->dict, environment_array_copy_cb, &copy);
	return ret;
error:
	strings_free(ret);
	return NULL;
}
