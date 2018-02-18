#!/usr/bin/env bash

# Copyright 2017,2018 cc-wrapper authors
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

# Outputs a C compatible header file enumerating all of the compiler
# executables in the environment. The structs are compatible with
# `struct exec_info` which can be found in src/execinfo.h
#
# Please refer to build/compiler_exes.sh for the input environment
# for running this script as it does the heavy lifting

set -e
set -o pipefail

# Load the compiler configuration
if ! compiler_tuples="$("$(dirname "$0")"/compiler_exes.sh)"; then
  exit 1
fi

# Includes
echo '#include <stddef.h>'
echo ''
echo '#include "config.h"'
echo '#include "execinfo.h"'

# Definitions
echo ''
echo 'const struct exec_info exec_infos[] = {'
oldifs="$IFS"
IFS=$'\n'
for tuple in $compiler_tuples; do
  oldifs2="$IFS"
  IFS=' '
  tuple=($tuple)
  echo $'\t''{'
  echo $'\t'$'\t'".name    = \"${tuple[0]}\","
  echo $'\t'$'\t'".path    = \"${tuple[1]}\","
  echo $'\t'$'\t'".package = \"${tuple[2]}\","
  echo $'\t'$'\t'".type    = \"${tuple[3]}\","
  echo $'\t'$'\t'".prefer  = ${tuple[4]},"
  echo $'\t''},'
  IFS="$oldifs2"
done
IFS="$oldifs"
echo $'\t''{'
echo $'\t'$'\t''.name    = NULL,'
echo $'\t''},'
echo '};'
