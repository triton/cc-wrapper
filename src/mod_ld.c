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
#include "config.h"
#include "log.h"
#include "mod_common.h"
#include "mod_ld.h"
#include "string-util.h"

bool ld_args_init(struct arguments *args, struct ld_args *ld_args)
{
	ld_args->args = args;
	ld_args->user_args_start = 0;
	ld_args->user_args_end = 0;
	for (size_t i = 0; i < arguments_nelems(args); ++i) {
		if (strcmp(CC_WRAPPER_USER_ARGS_BEGIN,
			   arguments_get(args, i)) == 0) {
			arguments_remove(args, i);
			ld_args->user_args_start = i--;
			continue;
		}
		if (strcmp(CC_WRAPPER_USER_ARGS_END, arguments_get(args, i)) ==
		    0) {
			arguments_remove(args, i);
			ld_args->user_args_end = i--;
			continue;
		}
	}

	/* We need to validate and fix the end */
	if (ld_args->user_args_end == 0)
		ld_args->user_args_end = arguments_nelems(args);
	if (ld_args->user_args_start > ld_args->user_args_end)
		return false;

	LOG_DEBUG("User args between: %lu and %lu\n", ld_args->user_args_start,
		  ld_args->user_args_end);
	return true;
}

bool ld_args_insert(struct ld_args *ld_args, size_t idx, const char *arg)
{
	if (idx > ld_args->user_args_start && idx < ld_args->user_args_end)
		return false;

	if (!arguments_insert(ld_args->args, idx, arg))
		return false;

	if (idx <= ld_args->user_args_start) {
		++ld_args->user_args_start;
		++ld_args->user_args_end;
	}
	return true;
}

static bool is_ld(const struct exec_info *exec_info)
{
	if (strcmp("ld", exec_info->type) == 0)
		return true;
	return false;
}

static bool replace_dl(struct ld_args *ld_args)
{
	if (target_dl == NULL)
		return true;

	LOG_DEBUG("Finding dynamic linker to replace with: %s\n", target_dl);
	for (size_t i = 0; i < ld_args->user_args_start; ++i)
		if (strcmp("-dynamic-linker",
			   arguments_get(ld_args->args, i)) == 0) {
			LOG_DEBUG("Replacing %s with %s\n",
				  arguments_get(ld_args->args, i + 1),
				  target_dl);
			if (!arguments_set(ld_args->args, i + 1, target_dl))
				return false;
		}

	return true;
}

static bool rpath_all_cc_paths(struct ld_args *ld_args)
{
	LOG_DEBUG("Adding rpaths to all -L switches passed by gcc\n");
	for (size_t i = 0;; ++i) {
		if (i == ld_args->user_args_start)
			i = ld_args->user_args_end;
		if (i >= arguments_nelems(ld_args->args))
			break;

		const char *arg = arguments_get(ld_args->args, i);
		if (strncmp("-L", arg, 2) == 0) {
			LOG_DEBUG("Adding rpath for %s\n", arg + 2);
			if (!ld_args_insert(ld_args, ++i, "-rpath"))
				return false;
			if (!ld_args_insert(ld_args, ++i, arg + 2))
				return false;
		}
	}

	return true;
}

static bool add_libc_shared_path(struct ld_args *ld_args)
{
	if (target_libc_dynamic_libs == NULL)
		return true;

	char *arg = string_printf("-L%s", target_libc_dynamic_libs);
	if (arg == NULL)
		return false;

	if (!ld_args_insert(ld_args, arguments_nelems(ld_args->args), arg)) {
		free(arg);
		return false;
	}

	free(arg);
	return true;
}

static bool add_libc_static_path(struct ld_args *ld_args)
{
	if (target_libc_static_libs == NULL)
		return true;

	char *arg = string_printf("-L%s", target_libc_static_libs);
	if (arg == NULL)
		return false;

	if (!ld_args_insert(ld_args, arguments_nelems(ld_args->args), arg)) {
		free(arg);
		return false;
	}

	free(arg);
	return true;
}

bool mod_ld_rewrite(const struct exec_info *exec_info, struct arguments *args,
		    struct environment *env)
{
	if (!is_ld(exec_info))
		return true;

	struct ld_args ld_args;
	if (!ld_args_init(args, &ld_args))
		return false;

	/* We want to avoid adding arguments if there aren't any */
	if (arguments_nelems(args) <= 1)
		return true;

	if (!replace_dl(&ld_args))
		return false;

	if (!add_libc_shared_path(&ld_args))
		return false;
	if (!add_libc_static_path(&ld_args))
		return false;

	/* Make sure this comes after all library flag manipulations */
	if (!rpath_all_cc_paths(&ld_args))
		return false;

	(void)env;
	return true;
}
