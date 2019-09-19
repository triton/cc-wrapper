#!/bin/sh
set -e
set -f
set -u

replaceLink() {
  rm -f "$2"
  ln -srv "$1" "$2"
}

mkdir -p "$BINDIR" "$TOOLDIR"
for tool in $(cat "$1"); do
  replaceLink "$BINDIR"/cc-wrapper "$BINDIR"/"$TARGET${TARGET:+-}$tool"
  replaceLink "$BINDIR"/cc-wrapper "$TOOLDIR"/"$tool"
done
