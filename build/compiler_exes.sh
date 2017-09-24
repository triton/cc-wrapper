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

getFull() {
  local oldpath="$PATH"
  export PATH="$COMPILER_PATH"
  if type -tP "$1"; then
    local st=0
  else
    local st="$?"
  fi
  export PATH="$oldpath"
  return "$st"
}

getAbs() {
  local full
  full="$(getFull "$1")"
  readlink -f "$full"
}

printTuple() {
  local abs
  if ! abs="$(getAbs "$2")"; then
    echo "Failed to find: $2" >&2
    return 1
  fi
  echo "$1 $abs"
}

# Figure out which compiler we are using
if getFull gcc >/dev/null; then
  compiler="gcc"
  printTuple cpp     cpp
  printTuple gcc     gcc
  printTuple cc      gcc
  printTuple g++     g++
  printTuple c++     g++
elif getFull clang >/dev/null; then
  compiler="clang"
  printTuple cpp     clang
  printTuple clang   clang
  printTuple cc      clang
  printTuple clang++ clang
  printTuple c++     clang
else
  echo "Unsupported compiler" >&2
  exit 1
fi

echo "Using compiler: $compiler" >&2

# Figure out which linker we are using
if getFull ld.bfd >/dev/null; then
  linker="binutils"
  printTuple ld.bfd  ld.bfd
  printTuple ld.gold ld.gold
elif getFull ld.lld >/dev/null && [ "$compiler" = "clang" ]; then
  linker="lld"
  printTuple ld.lld  ld.lld
  printTuple lld     lld
else
  echo "Unsupported linker" >&2
  exit 1
fi

echo "Using linker: $linker" >&2
