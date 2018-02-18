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

#ifndef CONFIG_H
#define CONFIG_H

#include "execinfo.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The list of binaries that the cc-wrapper can wrap
 */
extern const struct exec_info exec_infos[];

/*
 * The dynamic linker used during the link phase
 * This replaces the one passed from the compiler to the linker
 */
extern const char *target_dl;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // CONIG_H
