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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "arguments.h"
#include "config.h"
#include "environment.h"
#include "execinfo.h"
#include "log.h"
#include "mod_cc.h"
#include "mod_common.h"
#include "mod_ld.h"
#include "path.h"

#define MAIN_BINARY "cc-wrapper"

extern char **environ;

void set_log_level()
{
	/* By default we want to be mostly silent and not see info messages */
	log_level = LOG_LEVEL_ERROR;

	/* If a more specific log level is set we should use that instead */
	const char *level_str = getenv("CC_WRAPPER_LOG_LEVEL");
	if (level_str != NULL) {
		enum log_level specific_level =
		    log_level_from_string(level_str);
		if (specific_level != LOG_LEVEL_UNKNOWN) {
			log_level = specific_level;
		} else {
			LOG_ERROR("Invalid log level `%s`, valid levels are:\n",
				  level_str);
			log_levels_print(LOG_LEVEL_FATAL);
		}
	}

	LOG_INFO("Log level: %s\n", log_level_to_string(log_level));
}

void execute(const struct exec_info *exec_info, const struct arguments *args,
	     const struct environment *env)
{
	LOG_INFO("Calling `%s` with arguments:\n", exec_info->path);
	if (log_level >= LOG_LEVEL_INFO)
		arguments_print(args, log_level);
	LOG_INFO("and environment:\n");
	if (log_level >= LOG_LEVEL_INFO)
		environment_print(env, log_level);
	LOG_INFO("####################################\n");
	execve(exec_info->path, arguments_array_copy(args),
	       environment_array_copy(env));
}

bool should_print_config()
{
	const char *env = getenv("CC_WRAPPER_PRINT_CONFIG");
	return env != NULL && strcmp("1", env) == 0;
}

void print_config(FILE *file)
{
	if (target_dl)
		fprintf(file, "TARGET_DL=%s\n", target_dl);
	if (target_libc_include)
		fprintf(file, "TARGET_LIBC_INCLUDE=%s\n", target_libc_include);
	if (target_libc_dynamic_libs)
		fprintf(file, "TARGET_DYNAMIC_LIBS=%s\n",
			target_libc_dynamic_libs);
	if (target_libc_static_libs)
		fprintf(file, "TARGET_STATIC_LIBS=%s\n",
			target_libc_static_libs);
}

bool is_main_binary(const char *path)
{
	return strcmp(path_base(path), MAIN_BINARY) == 0;
}

int main(int argc, char *argv[])
{
	int ret = EXIT_FAILURE;
	struct arguments *args = NULL;
	struct environment *env = NULL;

	set_log_level();

	if (should_print_config()) {
		print_config(stdout);
		ret = EXIT_SUCCESS;
		goto out;
	}

	/* Determine the name of our target executable*/
	if (argc >= 1 && is_main_binary(argv[0])) {
		argc -= 1;
		argv += 1;
	}

	if (argc < 1) {
		LOG_ERROR("We must have a binary name\n");
		goto out;
	}
	LOG_DEBUG("Using binary name: %s\n", argv[0]);

	/* Print out our internal config */
	LOG_INFO("Internal cc-wrapper configuration:\n");
	if (LOG_LEVEL_INFO <= log_level)
		print_config(stderr);

	/* Get information about the executable we found */
	const struct exec_info *exec_info = get_exec_info(exec_infos, argv[0]);
	if (exec_info == NULL) {
		LOG_ERROR("Failed to get exec info for: %s\n", argv[0]);
		goto out;
	}
	print_exec_info(exec_info);

	/* Make a copy of our arguments and environment to manipulate */
	args = arguments_from_array((const char *const *)argv);
	if (args == NULL) {
		LOG_ERROR("Failed to make a copy of arguments\n");
		goto out;
	}
	env = environment_from_array((const char *const *)environ);
	if (env == NULL) {
		LOG_ERROR("Failed to make a copy of environ\n");
		goto out;
	}

	/* Print out some info to help debugging */
	LOG_INFO("Got initial arguments:\n");
	if (log_level >= LOG_LEVEL_INFO)
		arguments_print(args, log_level);
	LOG_INFO("Got initial environment:\n");
	if (log_level >= LOG_LEVEL_INFO)
		environment_print(env, log_level);

	/* Execute handlers for rewriting the environment */
	if (!mod_common_rewrite(exec_info, args, env)) {
		LOG_ERROR("Failed to do common rewrite\n");
		goto out;
	}
	if (!mod_cc_rewrite(exec_info, args, env)) {
		LOG_ERROR("Failed to do cc rewrite\n");
		goto out;
	}
	if (!mod_ld_rewrite(exec_info, args, env)) {
		LOG_ERROR("Failed to do ld rewrite\n");
		goto out;
	}

	execute(exec_info, args, env);

	/* We should never reach here */
	LOG_FATAL("Went past exec!\n");

out:
	arguments_free(args);
	environment_free(env);
	return ret;
}
