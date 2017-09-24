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

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "execinfo.h"
#include "config.h"

void set_log_level()
{
	const char *debug = getenv("NIX_DEBUG");
	if (debug != NULL && strcmp("1", debug) == 0)
		log_level = LOG_LEVEL_INFO;
	else
		log_level = LOG_LEVEL_WARN;
}

void print_args(char * const *args)
{
	for (size_t i = 0; args[i] != NULL; ++i)
		LOG_INFO("  %s\n", args[i]);
	LOG_INFO("\n");
}

void execute(const struct exec_info *exec_info, char * const *args)
{
	LOG_INFO("Calling `%s` with arguments:\n", exec_info->path);
	print_args(args);
	LOG_INFO("####################################\n");
	execv(exec_info->path, args);
}

int main(int argc, char *argv[])
{
	set_log_level();

	LOG_INFO("Got initial arguments:\n");
	print_args(argv);

	char **args = malloc(sizeof(char *) * (argc + 1));
	if (args == NULL)
		LOG_FATAL("Failed to allocate args list\n");

	for (size_t i = 0; argv[i] != NULL; ++i)
		args[i] = argv[i];

	const struct exec_info *exec_info = get_exec_info(exec_infos, argv[0]);
	if (exec_info == NULL)
		LOG_FATAL("Failed to get exec info for: %s\n", argv[0]);

	print_exec_info(exec_info);
	execute(exec_info, args);
}
