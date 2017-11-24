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

#include <stdlib.h>

#include "array.h"

#define MIN_SIZE (128 - 3)

struct array {
	void *data;
	size_t data_size;

	size_t elem_size;
	size_t nelems;
};

size_t best_fit_size(size_t exact_size)
{
	size_t ret = MIN_SIZE;
	while (ret < exact_size)
		ret <<= 1;
	return ret;
}

struct array *array_init(size_t elem_size, size_t nelems)
{
	struct array *array;

	array = malloc(sizeof(struct array));
	if (array == NULL)
		goto error;

	array->elem_size = elem_size;
	array->nelems = nelems;
	array->data_size = best_fit_size(elem_size * nelems);

	array->data = malloc(array->data_size);
	if (array->data == NULL)
		goto error;

	return array;

error:
	array_free(array);
	return NULL;
}

void array_free(struct array *array)
{
	if (array != NULL)
		free(array->data);
	free(array);
}

bool array_resize(struct array *array, size_t nelems)
{
	size_t new_size = best_fit_size(array->elem_size * nelems);
	void *new_data = NULL;

	if (new_size == array->data_size) {
		array->nelems = nelems;
		return true;
	}
	
	if (best_fit_size(new_size + 1) == array->data_size) {
		array->nelems = nelems;
		return true;
	}

	new_data = realloc(array->data, new_size);
	if (new_data == NULL)
		return false;

	array->nelems = nelems;
	array->data_size = new_size;
	array->data = new_data;
	return true;
}

size_t array_nelems(struct array *array)
{
	return array->nelems;
}

void *array_data(struct array *array)
{
	return array->data;
}

size_t array_allocation(struct array *array)
{
	return array->data_size;
}
