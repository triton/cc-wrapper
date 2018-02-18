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

#include <stdlib.h>
#include <string.h>

#include "arguments.h"
#include "array.h"
#include "log.h"
#include "string-util.h"

struct arguments {
	struct array *data;
};

struct arguments *arguments_from_array(const char *const argv[])
{
	struct arguments *args = malloc(sizeof(struct arguments));
	if (args == NULL)
		goto error;

	args->data = array_init(sizeof(char *), 0);
	if (args->data == NULL)
		goto error;

	int i = 0;
	for (; argv[i] != NULL; ++i) {
		if (!array_resize(args->data, i + 1))
			goto error;
		char **args_data = array_data(args->data);
		args_data[i] = string_clone(argv[i]);
	}

	if (!array_resize(args->data, i + 1))
		goto error;

	char **args_data = array_data(args->data);
	args_data[i] = NULL;

	return args;
error:
	arguments_free(args);
	return NULL;
}

void arguments_free(struct arguments *args)
{
	if (args == NULL)
		return;

	string_array_free(args->data);
	free(args);
}

void arguments_print(const struct arguments *args, enum log_level log_level)
{
	for (size_t i = 0; i < array_nelems(args->data) - 1; ++i)
		if (log_level >= LOG_LEVEL_DEBUG)
			log_printf(log_level, "  %3lu - %s\n", i,
				   arguments_array(args)[i]);
		else
			log_printf(log_level, "  %s\n",
				   arguments_array(args)[i]);
	log_printf(log_level, "\n");
}

size_t arguments_nelems(const struct arguments *args)
{
	return array_nelems(args->data) - 1;
}

const char *arguments_get(const struct arguments *args, size_t idx)
{
	if (idx >= arguments_nelems(args))
		return NULL;
	return arguments_array(args)[idx];
}

bool arguments_insert(struct arguments *args, size_t idx, const char *arg)
{
	if (idx >= array_nelems(args->data))
		return false;

	char *new_arg = string_clone(arg);
	if (new_arg == NULL)
		goto error;

	if (!array_resize(args->data, array_nelems(args->data) + 1))
		goto error;

	char **data = array_data(args->data);
	for (size_t i = arguments_nelems(args); i > idx; --i)
		data[i] = data[i - 1];
	data[idx] = new_arg;

	return true;
error:
	free(new_arg);
	return false;
}

bool arguments_set(struct arguments *args, size_t idx, const char *arg)
{
	if (idx >= arguments_nelems(args))
		return false;

	char *new_arg = string_clone(arg);
	if (new_arg == NULL)
		return false;

	char **data = array_data(args->data);
	free(data[idx]);
	data[idx] = new_arg;
	return true;
}

bool arguments_remove(struct arguments *args, size_t idx)
{
	if (idx >= arguments_nelems(args))
		return false;

	if (!array_resize(args->data, array_nelems(args->data) - 1))
		return false;

	char **data = array_data(args->data);
	free(data[idx]);
	for (size_t i = idx; i < arguments_nelems(args); ++i)
		data[i] = data[i + 1];
	data[arguments_nelems(args)] = NULL;
	return true;
}

const char *const *arguments_array(const struct arguments *args)
{
	return array_data_const(args->data);
}

char **arguments_array_copy(const struct arguments *args)
{
	char **ret;

	ret = malloc(sizeof(char *) * array_nelems(args->data));
	if (ret == NULL)
		goto error;
	memset(ret, 0, sizeof(char *) * array_nelems(args->data));

	const char *const *args_data = array_data_const(args->data);
	for (size_t i = 0; args_data[i] != NULL; ++i)
		if ((ret[i] = string_clone(args_data[i])) == NULL)
			goto error;

	return ret;
error:
	strings_free(ret);
	return NULL;
}
