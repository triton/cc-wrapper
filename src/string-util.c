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

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "string-util.h"

char *string_clone(const char *str)
{
	size_t str_bytes = sizeof(char) * (strlen(str) + 1);

	char *ret = malloc(str_bytes);
	if (ret == NULL) {
		LOG_DEBUG("Failed to allocate string\n");
		return NULL;
	}

	return memcpy(ret, str, str_bytes);
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
