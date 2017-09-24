#!/bin/sh

# Copyright 2017 cc-wrapper authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -e

config_h='config.h'

# Load the compiler configuration
if ! compiler_tuples="$("$(dirname "$0")"/compiler_exes.sh)"; then
  exit 1
fi

echo '#ifndef CONFIG_H' >"$config_h"
echo '#define CONFIG_H' >>"$config_h"
echo '' >>"$config_h"

# Includes
echo '#include "execinfo.h"' >>"$config_h"

echo '' >>"$config_h"
echo '#ifdef __cplusplus' >>"$config_h"
echo 'extern "C" {' >>"$config_h"
echo '#endif' >>"$config_h"
echo '' >>"$config_h"

# Definitions
echo 'struct exec_info exec_infos[] = {' >>"$config_h"
oldifs="$IFS"
IFS=$'\n'
for tuple in $compiler_tuples; do
  oldifs2="$IFS"
  IFS=' '
  tuple=($tuple)
  echo $'\t''{' >>"$config_h"
  echo $'\t'$'\t'".name = \"${tuple[0]}\"," >>"$config_h"
  echo $'\t'$'\t'".path = \"${tuple[1]}\"," >>"$config_h"
  echo $'\t''},' >>"$config_h"
  IFS="$oldifs2"
done
IFS="$oldifs"
echo $'\t''{' >>"$config_h"
echo $'\t'$'\t''.name = NULL,' >>"$config_h"
echo $'\t''},' >>"$config_h"
echo '};' >>"$config_h"

echo '' >>"$config_h"
echo '#ifdef __cplusplus' >>"$config_h"
echo '}  // extern "C"' >>"$config_h"
echo '#endif' >>"$config_h"
echo '' >>"$config_h"
echo '#endif  // CONIG_H' >>"$config_h"
