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

# We don't want any installer artifacts in the source directory
cd "$HOME"

download_and_unpack() {
  local url="$2"
  local output="$1"

  mkdir -p "${output}.tmp"
  curl -L -o "${output}.file" "$url"
  if ! tar x -C "${output}.tmp" -f "${output}.file"; then
    rm -r "${output}.tmp" "${output}.file"
    return 1
  fi
  rm "${output}.file"

  if test -d "${output}.tmp"/* 2>/dev/null; then
    mv "${output}.tmp"/* "${output}"
  else
    mv "${output}.tmp" "${output}"
  fi
}

build_autotools() {
  pushd "$1" >/dev/null
  shift
  ./configure \
    --prefix "$LOCAL_PREFIX" \
    "$@"
  make "${MAKEFLAGS[@]}"
  make "${MAKEFLAGS[@]}" install
  popd >/dev/null
}

build_cmake() {
  pushd "$1" >/dev/null
  shift
  cmake \
    -DCMAKE_INSTALL_PREFIX="$LOCAL_PREFIX" \
    -DCMAKE_BUILD_TYPE=Release \
    "$@"
  make "${MAKEFLAGS[@]}"
  make "${MAKEFLAGS[@]}" install
  popd >/dev/null
}

install_make() {
  download_and_unpack "make" \
    "https://ftp.gnu.org/gnu/make/make-${MAKE_VERSION}.tar.bz2"
  build_autotools "make"
}
install_valgrind() {
  download_and_unpack "valgrind" \
    "ftp://sourceware.org/pub/valgrind/valgrind-${VALGRIND_VERSION}.tar.bz2"
  build_autotools "valgrind"
}
install_googletest() {
  download_and_unpack "googletest" \
    "https://github.com/google/googletest/archive/${GOOGLETEST_VERSION}.tar.gz"
  build_cmake "googletest" \
    -DBUILD_GTEST=ON \
    -DBUILD_GMOCK=OFF
}

install_make &
install_googletest &
if [ "$BUILD_AND_CHECK" = "1" ]; then
  install_valgrind &
fi

wait
