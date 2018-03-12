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
#include <sys/types.h>

#include "arguments.h"
#include "config.h"
#include "execinfo.h"
#include "log.h"
#include "mod_cc.h"
#include "mod_common.h"
#include "string-util.h"

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

static bool remove_debug(struct arguments *args)
{
	LOG_DEBUG("Removing debug flags\n");
	for (size_t i = 0; i < arguments_nelems(args); ++i) {
		if (strncmp("-g", arguments_get(args, i), 2) != 0)
			continue;
		if (!arguments_remove(args, i--))
			return false;
	}
	return true;
}

static bool add_lib_path(struct arguments *args, char flag,
			 const char *arg_path)
{
	if (arg_path == NULL)
		return true;

	bool ret = false;
	char *arg = string_printf("-%c%s", flag, arg_path);
	if (arg == NULL)
		goto out;

	LOG_DEBUG("Adding library path: %s\n", arg);
	if (!arguments_insert(args, arguments_nelems(args), arg))
		goto out;

	ret = true;
out:
	free(arg);
	return ret;
}

static bool add_libc(struct arguments *args)
{
	if (!add_lib_path(args, 'L', target_libc_dynamic_libs))
		return false;
	if (!add_lib_path(args, 'B', target_libc_static_libs))
		return false;
	return true;
}

static bool rewrite_if_linking(const struct exec_info *exec_info,
			       struct arguments *args)
{
	LOG_DEBUG("Checking if we could invoke the linker\n");
	if (strcmp("cpp", exec_info->type) == 0)
		return true;
	for (size_t i = 0; i < arguments_nelems(args); ++i)
		if (strcmp("-c", arguments_get(args, i)) == 0)
			return true;

	LOG_DEBUG("Linking is possible\n");

	if (!add_libc(args))
		return false;

	return true;
}

static bool flag_rewrite(struct arguments *args, const struct environment *env)
{
	LOG_DEBUG("Checking if we should rewrite flags\n");
	if (!should_rewrite_flags(env))
		return true;
	LOG_DEBUG("Rewriting flags\n");

	if (!remove_debug(args))
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

	/* This has to come before other argument rewrites to make sure we
	 * tag the argument list correctly
	 */
	if (!add_linker_user_wrapper(args))
		return false;

	if (!add_libc_include(args))
		return false;

	if (!rewrite_if_linking(exec_info, args))
		return false;

	if (!flag_rewrite(args, env))
		return false;

	return true;
}
