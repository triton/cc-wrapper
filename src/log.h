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

#ifndef LOG_H
#define LOG_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum log_level {
	LOG_LEVEL_NONE = 0,
	LOG_LEVEL_FATAL = 1,
	LOG_LEVEL_ERROR = 2,
	LOG_LEVEL_WARN = 3,
	LOG_LEVEL_INFO = 4,
	LOG_LEVEL_DEBUG = 5,
	LOG_LEVEL_TRACE = 6,
};

/*
 * The current level of log output. Any levels above this
 * should be ignored.
 */
extern enum log_level log_level;

/*
 * Returns a string that describes the log level
 */
const char *log_level_to_string(enum log_level log_level);

/*
 * Prints out the log lines if the level is below or matching the
 * current log level. If the input level is LOG_LEVEL_FATAL, this
 * will terminate the application regardless of the current level.
 */
void log_printf(enum log_level level, const char *format, ...);

/*
 * Shorthands to log_printf to make calling more convenient
 * LOG_{LEVEL} -> log_printf(LOG_LEVEL_{LEVEL}
 */
#define LOG_FATAL(...) \
	do { log_printf(LOG_LEVEL_FATAL, __VA_ARGS__); } while(0)
#define LOG_ERROR(...) \
	do { log_printf(LOG_LEVEL_ERROR, __VA_ARGS__); } while(0)
#define LOG_WARN(...) \
	do { log_printf(LOG_LEVEL_WARN, __VA_ARGS__); } while(0)
#define LOG_INFO(...) \
	do { log_printf(LOG_LEVEL_INFO, __VA_ARGS__); } while(0)
#define LOG_DEBUG(...) \
	do { log_printf(LOG_LEVEL_DEBUG, __VA_ARGS__); } while(0)
#define LOG_TRACE(...) \
	do { log_printf(LOG_LEVEL_TRACE, __VA_ARGS__); } while(0)

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // LOG_H
