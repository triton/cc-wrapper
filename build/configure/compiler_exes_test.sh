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

SCRIPT="$(readlink -f "$(dirname "$0")")/compiler_exes.sh"
RES="$(readlink -f "$(dirname "$0")")/compiler_exes_test_res"
source "$(dirname "$0")/test_common.sh"

compiler_exes() {
  local OLDPATH="$PATH"
  export PATH="/no-such-path:$TMPDIR/bin"
  local st=0
  "$SCRIPT" || st="$?"
  export PATH="$OLDPATH"
  return "$st"
}

bin_create_bfd() {
  local file="$1"
  local abs="$(bin_path "$file")"

  bin_create "$file"
  echo '[ "$1" = "-v" ] || exit 1' >>"$abs"
  echo 'echo "GNU ld (GNU Binutils for Debian) 2.29.1"' >>"$abs"
}

bin_create_gold() {
  local file="$1"
  local abs="$(bin_path "$file")"

  bin_create "$file"
  echo '[ "$1" = "-v" ] || exit 1' >>"$abs"
  echo 'echo "GNU gold (GNU Binutils for Debian 2.29.1) 1.14"' >>"$abs"
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

test_banner "GCC Missing binaries"
if compiler_exes >/dev/null; then
  echo "Missing binary should have failed"
  exit 1
fi

test_banner "Default GCC"
bin_create "g++"
compiler_exes | sort >"$TMPDIR"/default.gcc.bfd.stdout
diff "$TMPDIR"/res/default.gcc.bfd.stdout \
  "$TMPDIR"/default.gcc.bfd.stdout

test_banner "Default GCC Gold"
bin_unlink "ld"
bin_link_local "ld.gold" "ld"
compiler_exes | sort >"$TMPDIR"/default.gcc.gold.stdout
diff "$TMPDIR"/res/default.gcc.gold.stdout \
  "$TMPDIR"/default.gcc.gold.stdout

test_banner "Default linker no symlink"
bin_unlink "ld"
bin_create_bfd "ld"
compiler_exes | sort >"$TMPDIR"/default.gcc.bfd.stdout
diff "$TMPDIR"/res/default.gcc.bfd.stdout \
  "$TMPDIR"/default.gcc.bfd.stdout
bin_unlink "ld"
bin_create_gold "ld"
compiler_exes | sort >"$TMPDIR"/default.gcc.gold.stdout
diff "$TMPDIR"/res/default.gcc.gold.stdout \
  "$TMPDIR"/default.gcc.gold.stdout

test_banner "Missing default linker"
bin_unlink "ld"
if compiler_exes >/dev/null; then
  echo "binutils should require a default linker"
  exit 1
fi
bin_link_local "ld.gold" "ld"

test_banner "Can be missing gold or bfd"
bin_unlink "ld.bfd"
bin_unlink "ld.gold"
bin_unlink "ld"
bin_create_gold "ld"
compiler_exes | sort >"$TMPDIR"/default.gcc.gold_only.stdout
diff "$TMPDIR"/res/default.gcc.gold_only.stdout \
  "$TMPDIR"/default.gcc.gold_only.stdout
bin_create_bfd "ld"
compiler_exes | sort >"$TMPDIR"/default.gcc.bfd_only.stdout
diff "$TMPDIR"/res/default.gcc.bfd_only.stdout \
  "$TMPDIR"/default.gcc.bfd_only.stdout

test_banner "Gcc no lld"
bin_unlink "ld"
bin_link_local "ld.lld" "ld"
if compiler_exes >/dev/null; then
  echo "Gcc shouldn't use lld"
  exit 1
fi
bin_create "ld.bfd"
if compiler_exes >/dev/null; then
  echo "Gcc shouldn't use lld"
  exit 1
fi
bin_unlink "ld.bfd"

bin_unlink "cpp"
bin_unlink "gcc"
bin_unlink "ld.lld"
bin_unlink "ld"
bin_unlink "g++"

test_banner "Default Clang"
bin_create "clang"
bin_create "lld"
bin_link_local "lld" "ld.lld"
bin_create "ld.bfd"
bin_create "ld.gold"
bin_link_local "ld.bfd" "ld"
compiler_exes | sort >"$TMPDIR"/default.clang.bfd.stdout
diff "$TMPDIR"/res/default.clang.bfd.stdout \
  "$TMPDIR"/default.clang.bfd.stdout

test_banner "Default Clang Gold"
bin_unlink "ld"
bin_link_local "ld.gold" "ld"
compiler_exes | sort >"$TMPDIR"/default.clang.gold.stdout
diff "$TMPDIR"/res/default.clang.gold.stdout \
  "$TMPDIR"/default.clang.gold.stdout

test_banner "Default Clang LLD"
bin_unlink "ld"
bin_link_local "ld.lld" "ld"
compiler_exes | sort >"$TMPDIR"/default.clang.lld.stdout
diff "$TMPDIR"/res/default.clang.lld.stdout \
  "$TMPDIR"/default.clang.lld.stdout
bin_unlink "ld"
bin_link_local "lld" "ld"
compiler_exes | sort >"$TMPDIR"/default.clang.lld.stdout
diff "$TMPDIR"/res/default.clang.lld.stdout \
  "$TMPDIR"/default.clang.lld.stdout

test_banner "Default Clang LLD no symlink"
bin_unlink "ld"
bin_create "ld"
echo '[ "$1" = "-v" ] || exit 1' >>"$TMPDIR"/bin/ld
echo 'echo "LLD 5.0.0 (compatible with GNU linkers)"' >>"$TMPDIR"/bin/ld
compiler_exes | sort >"$TMPDIR"/default.clang.lld.stdout
diff "$TMPDIR"/res/default.clang.lld.stdout \
  "$TMPDIR"/default.clang.lld.stdout

bin_unlink "clang"
bin_unlink "lld"
bin_unlink "ld.lld"
bin_unlink "ld.bfd"
bin_unlink "ld.gold"
bin_unlink "ld"
