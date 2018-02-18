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

#include "arguments.h"
#include "mod_cc.h"

bool is_cc(const struct exec_info *exec_info)
{
	if (strcmp("cpp", exec_info->type) == 0)
		return true;
	if (strcmp("c", exec_info->type) == 0)
		return true;
	if (strcmp("c++", exec_info->type) == 0)
		return true;
	return false;
}

bool mod_cc_rewrite(const struct exec_info *exec_info, struct arguments *args,
		    struct environment *env)
{
	if (!is_cc(exec_info))
		return true;

	/* We want to avoid adding arguments if there aren't any */
	if (arguments_nelems(args) <= 1)
		return true;

	(void)env;
	return true;
}
