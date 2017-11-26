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


# Setup the place where we install our local tools
LOCAL_PREFIX="$HOME/local-prefix"
mkdir -p "$LOCAL_PREFIX"
export PATH="$LOCAL_PREFIX/bin${PATH+:}$PATH"
export PKG_CONFIG_PATH="$LOCAL_PREFIX/lib/pkgconfig:$LOCAL_PREFIX/share/pkgconfig${PKG_CONFIG_PATH+:}$PKG_CONFIG_PATH"

# Setup flags for all of the make commands
declare -a MAKEFLAGS
MAKEFLAGS=()
declare -a NINJAFLAGS
NINJAFLAGS=()

# We want to make sure we keep going during builds
MAKEFLAGS+=("-k")
NINJAFLAGS+=("-k" "0")

# Setup the level of make parallelism by counting processors
NUM_CPUS="$(cat /proc/cpuinfo | grep '^processor' | wc -l)"
if [ "$NUM_CPUS" -lt "4" ]; then
  # We always want some level of parellelism to test the robustness
  # of the build system for parallel operations
  MAKEFLAGS+=("-j" "4")
  NINJAFLAGS+=("-j" "4")
else
  MAKEFLAGS+=("-j" "$NUM_CPUS")
  NINJAFLAGS+=("-j" "$NUM_CPUS")
fi

# Fixup the compiler variables
if [ "$CC_NAME" = "gcc" ]; then
  export CC="gcc-${CC_VERSION}"
  export CPP="gcc-${CC_VERSION} -E"
  export CXX="g++-${CC_VERSION}"
elif [ "$CC_NAME" = "clang" ]; then
  export CC="clang-${CC_VERSION}"
  export CPP="clang-${CC_VERSION} -E"
  export CXX="clang++-${CC_VERSION}"
  # TODO: Figure out how to get a proper libc++
  #export CXXFLAGS="$CXXFLAGS -stdlib=libc++"
else
  echo "Unsupported Compiler" >&2
  exit 1
fi
