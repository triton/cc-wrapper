set -e
set -o pipefail

declare -a -r TEST_EXIT_SIGNALS=(EXIT ERR INT TERM QUIT)
test_exit() {
  status="$?"
  if [ -n "$TMPDIR" ]; then
    rm -rf "$TMPDIR"
  fi
  trap - "${TEST_EXIT_SIGNALS[@]}"
  exit "$status"
}
trap test_exit "${TEST_EXIT_SIGNALS[@]}"
declare -r TMPDIR="$(readlink -f "$(mktemp -d "$(basename "$0").XXXXXXXXXX")")"

# Fixup all of our result files
mkdir -p "$TMPDIR"/res
for file in $(echo "$RES"/*); do
  sed "s,@TMPDIR@,$TMPDIR,g" "$file" >"$TMPDIR"/res/"$(basename "$file")"
done

# We want to control our default bin directory in the path
mkdir -p "$TMPDIR"/bin

test_banner() {
  echo "########## $1 ##########"
}

bin_path() {
  local exe="$1"
  echo "$TMPDIR"/bin/"$exe"
}

bin_link_exe() {
  local exe="$1"
  ln -s "$(type -tP "$exe")" "$(bin_path "$exe")"
}

bin_link_local() {
  local target="$1"
  local exe="$2"
  ln -s "$target" "$(bin_path "$exe")"
}

bin_create() {
  local exe="$1"
  echo "#!$(type -tP "bash")" >"$(bin_path "$exe")"
  chmod +x "$(bin_path "$exe")"
}

bin_unlink() {
  local exe="$1"
  rm "$(bin_path "$(basename "$exe")")"
}

# We need some basic dependencies
for dep in bash dirname basename grep readlink rm; do
  bin_link_exe "$dep"
done

