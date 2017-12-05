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

# Returns compiler executable tuples in the form
#   (exe name) (full exe path) (preferred binary level)
#   ...
#
# A preferred binary is used in the case of {exe}.{type}
# like ld.bfd or ld.gold. The list of {exe} will be sorted to
# choose the {type} with the highest value for preferred.
#
# Takes input from the environment variables:
#   TARGET_PATH: A colon(:) delimited path of compiler and linker
#     executables
#   TARGET_COMPILER: The name of the compiler.
#     Ex `gcc`, `clang`, `gcc-7`, `clang-5.0`
#   TARGET_LINKER: The name of the linker.
#     Ex `ld.bfd`, `ld.lld`
#   TARGET_ARCH: The prefix used for the compiler or target in clang.
#     Ex `aarch64-linux-gnu`, `x86_64-linux-gnu`

set -e
set -o pipefail

# Converts a named binary into the full path to that binary
#   getFull <bin_name>
#   Returns a string of the full path and code 0 on success
getFull() {
  local bin_name="$1"

  local oldpath="$PATH"
  export PATH="$target_path"
  if type -P "$1"; then
    local st=0
  else
    local st="$?"
  fi
  export PATH="$oldpath"
  return "$st"
}

# Converts a named binary into the absolute path to that binary
# following any symlinks along the way
#   getAbs <bin_name>
#   Returns the absolute path and code 0 on success
getAbs() {
  local bin_name="$1"

  local bin_full
  bin_full="$(getFull "$bin_name")"
  readlink -f "$bin_full"
}

# Prints a tuple representing the input binary
#   printTuple <exe_name> <searched_bin_name> <preference_level>
#   Returns tuples in the above format and code 0 on success
printTuple() {
  local exe_name="$1"
  local searched_bin_name="$2"
  local preference_level="$3"

  local abs
  if ! abs="$(getAbs "$searched_bin_name")"; then
    echo "Failed to find: $searched_bin_name" >&2
    return 1
  fi

  echo "$exe_name $abs $preference_level"
}

gccSupportArch() {
  local cc="$1"
  local arch="$2"

  [ "$("$cc" -dumpmachine)" = "$arch" ]
}

clangSupportArch() {
  local cc="$1"
  local arch="$2"

  echo "" | "$cc" -target "$arch" -x c -c -o - - >/dev/null 2>&1
}

canUseCompiler() {
  local compiler="$1"

  if echo "$TARGET_COMPILER" | grep -q "$compiler"; then
    return 0
  fi

  if [ -n "$TARGET_COMPILER" ]; then
    return 1
  fi

  if [ -n "$TARGET_ARCH" ] && getFull "$TARGET_ARCH"-"$compiler" >/dev/null; then
    return 0
  fi

  local cc
  if ! cc="$(getFull "$compiler")"; then
    return 1
  fi

  if [ -z "$TARGET_ARCH" ]; then
    return 0
  fi

  "${compiler}SupportArch" "$cc" "$TARGET_ARCH"
}

# Allow target path to be overriden from the PATH
if [ -z "$TARGET_PATH" ]; then
  target_path="$PATH"
else
  target_path="$TARGET_PATH"
fi

target_prefix="${TARGET_ARCH}${TARGET_ARCH:+-}"

# Figure out which compiler we are using
if canUseCompiler 'gcc'; then
  compiler="gcc"
  compiler_suffix="${TARGET_COMPILER:3}"
  printTuple "$target_prefix"cpp"$compiler_suffix"     "$target_prefix"cpp"$compiler_suffix"     0 || \
    printTuple "$target_prefix"cpp"$compiler_suffix"     "$target_prefix"g++"$compiler_suffix"     0
  printTuple "$target_prefix"gcc"$compiler_suffix"     "$target_prefix"gcc"$compiler_suffix"     0
  printTuple "$target_prefix"cc"$compiler_suffix"      "$target_prefix"gcc"$compiler_suffix"     0
  printTuple "$target_prefix"g++"$compiler_suffix"     "$target_prefix"g++"$compiler_suffix"     0
  printTuple "$target_prefix"c++"$compiler_suffix"     "$target_prefix"g++"$compiler_suffix"     0
elif canUseCompiler 'clang'; then
  compiler="clang"
  compiler_suffix="${TARGET_COMPILER:5}"
  if getFull "${target_prefix}clang${compiler_suffix}" >/dev/null; then
    clang_binary="${target_prefix}clang${compiler_suffix}"
  else
    clang_binary="clang${compiler_suffix}"
  fi
  printTuple "$target_prefix"cpp"$compiler_suffix"     "$clang_binary"   0
  printTuple "$target_prefix"clang"$compiler_suffix"   "$clang_binary"   0
  printTuple "$target_prefix"cc"$compiler_suffix"      "$clang_binary"   0
  printTuple "$target_prefix"clang++"$compiler_suffix" "$clang_binary"   0
  printTuple "$target_prefix"c++"$compiler_suffix"     "$clang_binary"   0
else
  echo "Unsupported compiler" >&2
  exit 1
fi

echo "Using compiler: $compiler" >&2

# Figure out which linker we are using
ld="$(getAbs ld)" || true
default_ld="$(basename "$ld")" || true
if [ "$default_ld" = "ld.gold" ]; then
  linker="binutils"
  printTuple ld.bfd  ld.bfd  0 2>/dev/null || true
  printTuple ld.gold ld.gold 1
elif [ "$default_ld" = "ld.bfd" ]; then
  linker="binutils"
  printTuple ld.bfd  ld.bfd  1
  printTuple ld.gold ld.gold 0 2>/dev/null || true
elif [ "$default_ld" = "ld.lld" ] && [ "$compiler" = "clang" ]; then
  linker="lld"
  printTuple ld.lld  ld.lld  1
elif [ "$default_ld" = "lld" ] && [ "$compiler" = "clang" ]; then
  linker="lld"
  printTuple ld.lld  ld.lld  1
elif [ -n "$ld" ] && "$ld" -v | grep -q '^GNU gold'; then
  linker="binutils"
  printTuple ld.bfd  ld.bfd  0 2>/dev/null || true
  printTuple ld.gold ld.gold 1 2>/dev/null || printTuple ld.gold ld 1
elif [ -n "$ld" ] && "$ld" -v | grep -q '^GNU ld'; then
  linker="binutils"
  printTuple ld.bfd  ld.bfd  1 2>/dev/null || printTuple ld.bfd ld 1
  printTuple ld.gold ld.gold 0 2>/dev/null || true
elif [ -n "$ld" ] && "$ld" -v | grep -q '^LLD' && [ "$compiler" = "clang" ]; then
  linker="lld"
  printTuple ld.lld  ld.lld  1
else
  echo "Unsupported Linker" >&2
  exit 1
fi

echo "Using linker: $linker" >&2
