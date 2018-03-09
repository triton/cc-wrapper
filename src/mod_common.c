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

#include <stdbool.h>
#include <string.h>

#include "environment.h"
#include "mod_common.h"
#include "string-util.h"

static bool add_our_path(struct environment *env)
{
	char *path_new = NULL;
	const char *path_old = environment_get(env, "PATH");
	if (path_old == NULL || path_old[0] == '\0')
		/* Avoid including the current directory in the path */
		path_new = string_clone(CC_WRAPPER_BIN);
	else
		/* Our binaries should prioritize calling each other over those
		 * in the provided path.
		 */
		path_new = string_printf("%s:%s", CC_WRAPPER_BIN, path_old);

	if (path_new == NULL)
		return false;

	LOG_DEBUG("Adding our path to PATH: %s\n", path_new);
	bool ret = environment_set(env, "PATH", path_new);
	free(path_new);
	return ret;
}

bool mod_common_rewrite(const struct exec_info *exec_info,
			struct arguments *args, struct environment *env)
{
	if (!add_our_path(env))
		return false;

	(void)exec_info;
	(void)args;
	return true;
}
