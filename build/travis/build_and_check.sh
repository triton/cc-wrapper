#!/usr/bin/env bash

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

# Outputs a C compatible header file enumerating all of the compiler
# executables in the environment. The structs are compatible with
# `struct exec_info` which can be found in src/execinfo.h
#
# Please refer to build/compiler_exes.sh for the input environment
# for running this script as it does the heavy lifting

set -e
set -o pipefail
set -x

source "$(dirname "$0")/common.sh"

# Print out the relevant versions of our applications
valgrind --version
$CC --version
ld --version
ar --version
make --version

# Make sure output is not interleaved in parallel execution
MAKEFLAGS+=("-O")

# Run through the typical build process
PREFIX="$LOCAL_PREFIX" ./configure
make "${MAKEFLAGS[@]}" all
make "${MAKEFLAGS[@]}" check
make "${MAKEFLAGS[@]}" install

# Make sure we have installed the cc-wrapper
test -f "$LOCAL_PREFIX"/bin/cc-wrapper

# Make sure clean removes objects but not generated files
make "${MAKEFLAGS[@]}" clean
if [ -n "$(find . -name \*.o -or -name cc-wrapper)" ]; then
  echo "Found files that should have been cleaned" >&2
  exit 1
fi
make "${MAKEFLAGS[@]}" all

# Make sure distclean removes generated files
make "${MAKEFLAGS[@]}" distclean
if [ -n "$(find . -name \*.gen)" ]; then
  echo "We should not find gen files after a distclean" >&2
  exit 1
fi
if [ -n "$(find . -name \*.o -or -name cc-wrapper)" ]; then
  echo "Found files that should have been cleaned" >&2
  exit 1
fi
if make "${MAKEFLAGS[@]}" all; then
  echo "Make after a distclean should fail" >&2
  exit 1
fi

# Make sure the installed cc-wrapper works
"$LOCAL_PREFIX"/bin/cc-wrapper gcc --version
"$LOCAL_PREFIX"/bin/gcc --version
"$LOCAL_PREFIX"/bin/g++ --version
"$LOCAL_PREFIX"/bin/ld.bfd --version
"$LOCAL_PREFIX"/bin/ld --version
"$LOCAL_PREFIX"/bin/cpp --version
