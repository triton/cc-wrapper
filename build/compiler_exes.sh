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

# Returns compiler executable tuples in the form
#   (exe name) (full exe path) (preferred binary)
#   ...
#
# A preferred binary is used in the case of {exe}.{type}
# like ld.bfd or ld.gold. The list of {exe} will be sorted to
# choose the {type} with the highest value for preferred.

set -e

if [ "$1" = "--exe-only" ]; then
  exeOnly="1"
fi

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
  if [ "$exeOnly" = "1" ]; then
    echo "$1"
  else
    echo "$1 $abs $3"
  fi
}

# Figure out which compiler we are using
if getFull gcc >/dev/null; then
  compiler="gcc"
  printTuple cpp     cpp     0
  printTuple gcc     gcc     0
  printTuple cc      gcc     0
  printTuple g++     g++     0
  printTuple c++     g++     0
elif getFull clang >/dev/null; then
  compiler="clang"
  printTuple cpp     clang   0
  printTuple clang   clang   0
  printTuple cc      clang   0
  printTuple clang++ clang   0
  printTuple c++     clang   0
else
  echo "Unsupported compiler" >&2
  exit 1
fi

echo "Using compiler: $compiler" >&2

# Figure out which linker we are using
if getFull ld.bfd >/dev/null; then
  linker="binutils"
  default_ld="$(basename "$(getAbs ld)")"
  if [ "$default_ld" = "ld.gold" ]; then
    printTuple ld.bfd  ld.bfd  0
    printTuple ld.gold ld.gold 1
  elif [ "$default_ld" = "ld.bfd" ]; then
    printTuple ld.bfd  ld.bfd  1
    printTuple ld.gold ld.gold 0
  else
    echo "No default linker found" >&2
    exit 1
  fi
elif getFull ld.lld >/dev/null && [ "$compiler" = "clang" ]; then
  linker="lld"
  printTuple ld.lld  ld.lld  1
  printTuple lld     lld     0
else
  echo "Unsupported linker" >&2
  exit 1
fi

echo "Using linker: $linker" >&2
