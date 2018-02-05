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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "arguments.h"
#include "config.h"
#include "execinfo.h"
#include "log.h"
#include "path.h"

#define MAIN_BINARY "cc-wrapper"

void set_log_level()
{
	/* We want to figure out our base log level from the build environment
	 * for proper integration so it is trivial to get lots of useful build
	 * info
	 */
	const char *debug = getenv("NIX_DEBUG");
	if (debug != NULL && strcmp("1", debug) == 0)
		log_level = LOG_LEVEL_INFO;
	else
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

void execute(const struct exec_info *exec_info, const struct arguments *args)
{
	LOG_INFO("Calling `%s` with arguments:\n", exec_info->path);
	arguments_print(args, LOG_LEVEL_INFO);
	LOG_INFO("####################################\n");
	execv(exec_info->path, arguments_array_copy(args));
}

bool is_main_binary(const char *path)
{
	return strcmp(path_base(path), MAIN_BINARY) == 0;
}

int main(int argc, char *argv[])
{
	int ret = EXIT_FAILURE;
	struct arguments *args = NULL;

	set_log_level();

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

	/* Print out some info to help debugging */
	LOG_INFO("Got initial arguments:\n");
	arguments_print(args, LOG_LEVEL_INFO);

	/* Execute handlers for rewriting the environment */
	// TODO(wak)

	execute(exec_info, args);

	/* We should never reach here */
	LOG_FATAL("Went past exec!\n");

out:
	arguments_free(args);
	return ret;
}
