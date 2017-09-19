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

#include <stdlib.h>
#include <string.h>

#include "log.h"

void set_log_level()
{
	const char *debug = getenv("NIX_DEBUG");
	if (debug != NULL && strcmp("1", debug) == 0)
		log_level = LOG_LEVEL_DEBUG;
	else
		log_level = LOG_LEVEL_WARN;
}

int main(int argc, char *argv[])
{
	set_log_level();
	LOG_INFO("Got initial arguments:\n");
	for (int i = 0; i < argc; ++i)
		LOG_INFO("  %s\n", argv[i]);
	LOG_INFO("\n");
	return 1;
}
