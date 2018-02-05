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

#include <libgen.h>
#include <stddef.h>
#include <string.h>

#include "execinfo.h"
#include "log.h"
#include "path.h"

void print_exec_info(const struct exec_info *exec_info)
{
	LOG_DEBUG("EXEC_INFO:\n");
	LOG_DEBUG("  Name: %s\n", exec_info->name);
	LOG_DEBUG("  Path: %s\n", exec_info->path);
	LOG_DEBUG("  Type: %s\n", exec_info->type);
	LOG_DEBUG("  Prefer: %d\n", exec_info->prefer);
	LOG_DEBUG("\n");
}

const struct exec_info *get_exec_info(const struct exec_info *exec_infos,
				      const char *arg0)
{
	const char *name = path_base(arg0);
	const struct exec_info *ret = NULL;

	for (size_t i = 0; exec_infos[i].name != NULL; ++i) {
		// Exact matches always come first
		if (strcmp(name, exec_infos[i].name) == 0) {
			ret = &exec_infos[i];
			goto out;
		}

		// Fallback to {exe}.{type} matching
		size_t name_len = strlen(name);
		size_t i_len = strlen(exec_infos[i].name);
		if (name_len + 1 > i_len)
			continue;
		if (exec_infos[i].name[name_len] != '.')
			continue;
		if (strncmp(name, exec_infos[i].name, name_len) != 0)
			continue;
		if (ret == NULL || ret->prefer < exec_infos[i].prefer)
			ret = &exec_infos[i];
	}

out:
	return ret;
}
