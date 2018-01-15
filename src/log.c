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

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

enum log_level log_level = LOG_LEVEL_INFO;

void log_levels_print(enum log_level log_level)
{
	log_printf(log_level, "  NONE / none\n"
			      "  FATAL / fatal\n"
			      "  ERROR / error\n"
			      "  WARN / warn\n"
			      "  INFO / info\n"
			      "  DEBUG / debug\n"
			      "  TRACE / trace\n");
}

enum log_level log_level_from_string(const char *level_string)
{
	if (strcmp("NONE", level_string) == 0 ||
	    strcmp("none", level_string) == 0)
		return LOG_LEVEL_NONE;
	if (strcmp("FATAL", level_string) == 0 ||
	    strcmp("fatal", level_string) == 0)
		return LOG_LEVEL_FATAL;
	if (strcmp("ERROR", level_string) == 0 ||
	    strcmp("error", level_string) == 0)
		return LOG_LEVEL_ERROR;
	if (strcmp("WARN", level_string) == 0 ||
	    strcmp("warn", level_string) == 0)
		return LOG_LEVEL_WARN;
	if (strcmp("INFO", level_string) == 0 ||
	    strcmp("info", level_string) == 0)
		return LOG_LEVEL_INFO;
	if (strcmp("DEBUG", level_string) == 0 ||
	    strcmp("debug", level_string) == 0)
		return LOG_LEVEL_DEBUG;
	if (strcmp("TRACE", level_string) == 0 ||
	    strcmp("trace", level_string) == 0)
		return LOG_LEVEL_TRACE;
	return LOG_LEVEL_UNKNOWN;
}

const char *log_level_to_string(enum log_level log_level)
{
	switch (log_level) {
	case LOG_LEVEL_NONE:
		return "NONE";
	case LOG_LEVEL_FATAL:
		return "FATAL";
	case LOG_LEVEL_ERROR:
		return "ERROR";
	case LOG_LEVEL_WARN:
		return "WARN";
	case LOG_LEVEL_INFO:
		return "INFO";
	case LOG_LEVEL_DEBUG:
		return "DEBUG";
	case LOG_LEVEL_TRACE:
		return "TRACE";
	default:
		return "UNKNOWN";
	};
}

void log_printf(enum log_level level, const char *format, ...)
{
	va_list va_args;
	va_start(va_args, format);

	if (level <= log_level)
		vfprintf(stderr, format, va_args);
	va_end(va_args);

	if (level == LOG_LEVEL_FATAL)
		exit(EXIT_FAILURE);
}
