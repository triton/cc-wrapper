#!/bin/sh
set -e
set -f
set -u

valid_tools="as ar lld ld ld.bfd ld.gold ld.lld
             nm objdump objcopy ranlib readelf strip"
if [ "$PREFERRED_COMPILER" = 'no' -o "$PREFERRED_COMPILER" = 'gcc' ]; then
  valid_tools="$valid_tools gcc g++"
fi
if [ "$PREFERRED_COMPILER" = 'no' -o "$PREFERRED_COMPILER" = 'clang' ]; then
  valid_tools="$valid_tools clang clang++"
fi

linked_tools=""

extra_args="
cpp -E
"

types="
clang GCC_COMPILER
clang++ GCC_COMPILER
gcc GCC_COMPILER
g++ GCC_COMPILER
ld LINKER
ld.bfd LINKER
ld.gold LINKER
ld.lld LINKER
lld LINKER
"

syms="
clang cc cpp
clang++ c++
gcc cc cpp
g++ c++
"

prefix_map_flags="
clang $PREFIX_MAP_FLAG_CLANG
clang++ $PREFIX_MAP_FLAG_CLANG
gcc $PREFIX_MAP_FLAG_GCC
g++ $PREFIX_MAP_FLAG_GCC
"

get_type() {
  local t
  t="$(echo "$types" | grep "^$1 " | awk '{print $2}')"
  echo "${t:-GENERIC}"
}

get_extra_args() {
  echo "$extra_args" | grep "^$1 " | awk \
    'BEGIN { printf "{"; } { for(i = 2; i <= NF; i++) { printf "\"" $i "\","; } } END { printf "}"; }'
}

get_syms() {
  echo "$syms" | grep "^$1 " | awk '{$1=""; print $0}'
}

get_prefix_map_flag() {
  local f
  f="$(echo "$prefix_map_flags" | grep "^$1 " | awk '{print $2}')"
  if [ -n "$f" ]; then
    echo "\"$f\""
  else
    echo 'nonstd::nullopt'
  fi
}

exec 3>"$1"
exec 4>"$2"

echo '#include "bins.hpp"' >&3
echo '' >&3
echo 'namespace cc_wrapper {' >&3
echo 'namespace bins {' >&3
echo 'const InfoMap& getInfoMap() {' >&3
echo 'static InfoMap map = {' >&3
oldifs="$IFS"
IFS=":"
for dir in $TOOLDIRS; do
  IFS="$oldifs"
  for file in $(ls $dir); do
    valid=
    for tool in $valid_tools; do
      [ "$TARGET${TARGET:+-}$tool" = "$file" ] && valid="$tool"
    done
    [ -n "$valid" ] || continue
    adir="$(dirname "$(readlink -f "$dir/$file")")"
    t="$(get_type "$valid")"
    f="$(get_prefix_map_flag "$valid")"
    for s in "$valid" $(get_syms "$valid"); do
      linked=
      for tool in $linked_tools; do
        [ "$s" = "$tool" ] && linked=1
      done
      if [ -n "$linked" ]; then
        echo "WARNiNG: Skipping duplicate tool $s" >&2
        continue
      fi
      linked_tools="$linked_tools $s"
      args="$(get_extra_args "$s")"
      if [ -n "$TARGET" ]; then
        echo "{\"$TARGET-$s\", { \"$TARGET-$s\", Type::$t, \"$adir/$file\". $args, $f }}," >&3
      fi
      echo "{\"$s\", { \"$s\", Type::$t, \"$adir/$file\", $args, $f }}," >&3
      echo "$s" >&4
    done
  done
done
echo '};' >&3
echo 'return map;' >&3
echo '}' >&3
echo '}' >&3
echo '}' >&3
