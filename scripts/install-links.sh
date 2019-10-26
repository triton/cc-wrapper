#!/bin/sh
set -e
set -f
set -u

mkdir -p "$LINKDIR"
oldifs="$IFS"
IFS="${IFS#??}"
for line in $(cat "$1"); do
  IFS="$oldifs"
  tool="$(echo "$line" | awk '{print $1}')"
  file="$(echo "$line" | awk '{print $2}')"
  ln -sfv "$file" "$LINKDIR"/"$TARGET-$tool"
done
IFS="$oldifs"

replaceLink() {
  rm -f "$2"
  ln -srv "$1" "$2"
}

mkdir -p "$BINDIR" "$TOOLDIR"
cc_wrapper="$BINDIR"/$TARGET_ALIAS${TARGET_ALIAS:+-}cc-wrapper
for tool in $(cat "$1" | awk '{print $1}'); do
  if [ -z "$TARGET_ALIAS" ]; then
    replaceLink "$cc_wrapper" "$BINDIR"/"$tool"
  fi
  replaceLink "$cc_wrapper" "$BINDIR"/"$TARGET${TARGET:+-}$tool"
  replaceLink "$cc_wrapper" "$TOOLDIR"/"$tool"
done
if [ -z "$TARGET_ALIAS" ]; then
  replaceLink "$cc_wrapper" "$BINDIR"/${TARGET}-cc-wrapper
fi
