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
#include "mod_ld.h"

struct ld_args {
	struct arguments *args;
	size_t user_args_start;
	size_t user_args_end;
};

void ld_args_init(struct arguments *args, struct ld_args *ld_args)
{
	ld_args->args = args;
	ld_args->user_args_start = 0;
	ld_args->user_args_end = arguments_nelems(args);
	for (size_t i = 0; i < arguments_nelems(args); ++i) {
		if (strcmp("--cc-wrapper-begin", arguments_get(args, i)) == 0) {
			arguments_remove(args, i);
			ld_args->user_args_start = i--;
			continue;
		}
		if (strcmp("--cc-wrapper-end", arguments_get(args, i)) == 0) {
			arguments_remove(args, i);
			ld_args->user_args_end = i--;
			continue;
		}
	}

	LOG_DEBUG("User args between: %lu and %lu\n", ld_args->user_args_start,
		  ld_args->user_args_end);
}

bool is_ld(const struct exec_info *exec_info)
{
	if (strcmp("ld", exec_info->type) == 0)
		return true;
	return false;
}

bool replace_dl(struct ld_args *ld_args)
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
			break;
		}

	return true;
}

bool mod_ld_rewrite(const struct exec_info *exec_info, struct arguments *args,
		    struct environment *env)
{
	if (!is_ld(exec_info))
		return true;

	/* We want to avoid adding arguments if there aren't any */
	if (arguments_nelems(args) <= 1)
		return true;

	struct ld_args ld_args;
	ld_args_init(args, &ld_args);

	if (!replace_dl(&ld_args))
		return false;

	(void)env;
	return true;
}
