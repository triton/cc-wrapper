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

#include "arguments.h"
#include "config.h"
#include "execinfo.h"
#include "mod_cc.h"
#include "mod_common.h"

static bool is_cc(const struct exec_info *exec_info)
{
	if (strcmp("cpp", exec_info->type) == 0)
		return true;
	if (strcmp("c", exec_info->type) == 0)
		return true;
	if (strcmp("c++", exec_info->type) == 0)
		return true;
	return false;
}

static bool add_libc_include(struct arguments *args)
{
	if (target_libc_include == NULL)
		return true;
	for (size_t i = 0; i < arguments_nelems(args); ++i)
		if (strcmp("-nostdinc", arguments_get(args, i)) == 0)
			return true;

	if (!arguments_insert(args, 1, "-idirafter"))
		return false;
	if (!arguments_insert(args, 2, target_libc_include))
		return false;
	return true;
}

static bool add_linker_user_wrapper(struct arguments *args)
{
	if (!arguments_insert(args, 1, "-Wl," CC_WRAPPER_USER_ARGS_BEGIN))
		return false;
	if (!arguments_insert(args, arguments_nelems(args),
			      "-Wl," CC_WRAPPER_USER_ARGS_END))
		return false;
	return true;
}

bool mod_cc_rewrite(const struct exec_info *exec_info, struct arguments *args,
		    struct environment *env)
{
	if (!is_cc(exec_info))
		return true;

	/* We want to avoid adding arguments if there aren't any */
	if (arguments_nelems(args) <= 1)
		return true;

	if (!add_linker_user_wrapper(args))
		return false;

	if (!add_libc_include(args))
		return false;

	(void)env;
	return true;
}
