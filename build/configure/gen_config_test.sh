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

export LC_COLLATE=C
export LC_ALL=C

SCRIPT="$(readlink -f "$(dirname "$0")")/gen_config.sh"
RES="$(readlink -f "$(dirname "$0")")/gen_config_test_res"
source "$(dirname "$0")/test_common.sh"

gen_config() {
  local OLDPATH="$PATH"
  export PATH="/no-such-path:$TMPDIR/bin"
  local st=0
  "$SCRIPT" || st="$?"
  export PATH="$OLDPATH"
  return "$st"
}

# We should work with everything empty and use some sane defaults
unset TARGET_PATH
unset TARGET_COMPILER
unset TARGET_LINKER
unset TARGET_ARCH

bin_create "cpp"
bin_create "gcc"
bin_create "ld.bfd"
bin_create "ld.gold"
bin_create "ld.lld"
bin_link_local "ld.bfd" "ld"
bin_create "g++"

test_banner "Default GCC"
gen_config >"$TMPDIR"/default.gcc.bfd.stdout
diff "$TMPDIR"/res/default.gcc.bfd.stdout \
  "$TMPDIR"/default.gcc.bfd.stdout
