#!/bin/sh
set -e
set -f
set -u

replaceLink() {
  rm -f "$2"
  ln -srv "$1" "$2"
}

mkdir -p "$BINDIR" "$TOOLDIR"
cc_wrapper="$BINDIR"/$TARGET${TARGET:+-}cc-wrapper
for tool in $(cat "$1"); do
  replaceLink "$cc_wrapper" "$BINDIR"/"$TARGET${TARGET:+-}$tool"
  replaceLink "$cc_wrapper" "$TOOLDIR"/"$tool"
done
