/*
 * Copyright 2017,2018 cc-wrapper authors
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "string-util.h"

char *string_clone(const char *str)
{
	if (str == NULL)
		return NULL;

	size_t str_bytes = sizeof(char) * (strlen(str) + 1);

	char *ret = malloc(str_bytes);
	if (ret == NULL) {
		LOG_DEBUG("Failed to allocate string\n");
		return NULL;
	}

	return memcpy(ret, str, str_bytes);
}

char *string_clone_n(const char *str, size_t n)
{
	if (n == 0)
		return NULL;

	char *ret = malloc(n * sizeof(char));
	if (ret == NULL) {
		LOG_DEBUG("Failed to allocate string\n");
		return NULL;
	}

	size_t str_len = 0;
	if (str != NULL) {
		str_len = sizeof(char) * strlen(str);
		memcpy(ret, str, str_len < n ? str_len : n);
	}

	if (str_len < n)
		memset(ret + str_len, 0, n - str_len);

	return ret;
}

char *string_printf(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	const size_t ret_size = vsnprintf(NULL, 0, format, args) + 1;
	va_end(args);

	char *ret = malloc(sizeof(char) * ret_size);
	if (ret == NULL)
		return NULL;

	va_start(args, format);
	vsnprintf(ret, ret_size, format, args);
	va_end(args);
	return ret;
}

void string_array_free(struct array *arr)
{
	if (arr == NULL)
		return;

	char **arr_data = array_data(arr);
	for (size_t i = 0; i < array_nelems(arr); ++i)
		free(arr_data[i]);
	array_free(arr);
}

void strings_free(char **strings)
{
	if (strings == NULL)
		return;

	for (size_t i = 0; strings[i] != NULL; ++i)
		free(strings[i]);
	free(strings);
}
