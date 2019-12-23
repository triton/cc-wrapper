#!/bin/sh
set -e
set -f
set -u

valid_tools="as ar lld ld ld.bfd ld.gold ld.lld
             nm objdump objcopy ranlib readelf strip"
if [ "$PREFERRED_COMPILER" = 'no' -o "$PREFERRED_COMPILER" = 'gcc' ]; then
  valid_tools="$valid_tools gcc g++ gcc-ar gcc-nm gcc-ranlib"
fi
if [ "$PREFERRED_COMPILER" = 'no' -o "$PREFERRED_COMPILER" = 'clang' ]; then
  valid_tools="$valid_tools clang clang++"
fi

linked_tools=""

extra_args="
cpp -E
"

funcs="
clang gcc::ccMain
clang++ gcc::cxxMain
gcc gcc::ccMain
g++ gcc::cxxMain
gcc-ar gcc::ccMain
gcc-nm gcc::ccMain
gcc-ranlib gcc::ccMain
ld linker::main
ld.bfd linker::main
ld.gold linker::main
ld.lld linker::main
lld linker::main
"

info_func="
clang make_gcc_info
clang++ make_gcc_info
gcc make_gcc_info
g++ make_gcc_info
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

get_func() {
  local t
  t="$(echo "$funcs" | grep "^$1 " | awk '{print $2}')"
  echo "${t:-generic::main}"
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

get_info_func() {
  local f
  f="$(echo "$info_func" | grep "^$1 " | awk '{print $2}')"
  if [ -n "$f" ]; then
    echo "$f"
  else
    echo 'make_info'
  fi
}

prefixes() {
  echo "$TARGET-"
  if [ "$(echo "$TARGET" | tr '-' '\n' | wc -l)" -eq 4 ]; then
    echo "$TARGET" | awk -F- '{ print $1"-"$3"-"$4"-"; }'
  fi
  echo ""
}

_make_info() {
  local refname="$1"
  local tool="$2"
  local name="$3"
  local class="$4"
  local extra="${5-}"

  local f="$(get_func "$tool")"
  local args="$(get_extra_args "$name")"
  echo "map.emplace(\"$refname\", new $class(\"$TARGET${TARGET:+-}$name\", $f, $args$extra));"
}

make_info() {
  local tool="$1"
  local name="$2"
  shift 2
  local class="${1-Info}"
  if [ "$#" -gt 1 ]; then
    shift
  fi

  local pfx
  for pfx in $(prefixes) ""; do
    _make_info "$pfx$name" "$tool" "$name" "$class" "$@"
  done
}

make_gcc_info() {
  local tool="$1"

  local f="$(get_prefix_map_flag "$tool")"
  make_info "$@" GccInfo ", $f"
}

process_tools() {
  local pfx="$1"
  echo "Considering tools: $pfx" >&2

  local oldifs="$IFS"
  IFS=":"
  local dir
  for dir in $TOOLDIRS; do
    IFS="$oldifs"
    local file
    for file in $(ls $dir); do
      [ "$pfx" != "" ] && [ "${file#$pfx}" = "$file" ] && continue
      local valid=
      local tool
      for tool in $valid_tools; do
        [ "$pfx$tool" = "$file" ] && valid="$tool"
        [ "$tool" = "$file" ] && valid="$tool"
      done
      [ -n "$valid" ] || continue
      local afile
      afile="$(readlink -f "$dir/$file")"
      local func
      func="$(get_info_func "$valid")"
      local s
      for s in "$valid" $(get_syms "$valid"); do
        local linked=
        local tool
        for tool in $linked_tools; do
          [ "$s" = "$tool" ] && linked=1
        done
        if [ -n "$linked" ]; then
          echo "WARNiNG: Skipping duplicate tool $s" >&2
          continue
        fi
        echo "Linking tool: $s $afile" >&2
        linked_tools="$linked_tools $s"
        "$func" "$valid" "$s" >&3
        echo "$s $afile" >&4
      done
    done
  done
  IFS="$oldifs"
}

exec 3>"$1"
exec 4>"$2"

echo '#include <bins.hpp>' >&3
echo '#include <gcc.hpp>' >&3
echo '#include <generic.hpp>' >&3
echo '#include <linker.hpp>' >&3
echo '' >&3
echo 'namespace cc_wrapper {' >&3
echo 'namespace bins {' >&3
echo 'InfoMap makeInfoMap() {' >&3
echo 'InfoMap map;' >&3
for pfx in $(prefixes) ""; do
  process_tools "$pfx"
done
echo 'return map;' >&3
echo '}' >&3
echo 'const InfoMap& getInfoMap() {' >&3
echo 'static InfoMap map = makeInfoMap();' >&3
echo 'return map;' >&3
echo '}' >&3
echo '}' >&3
echo '}' >&3
