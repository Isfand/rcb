#!/bin/sh

SCRIPT_PATH="$(cd "$(dirname "$0")" && pwd)"
SOURCE_PATH="$(cd "$SCRIPT_PATH" && b info | awk '/^src_root:/ {print $2}')"
OUTPUT_PATH="$(cd "$SCRIPT_PATH" && bdep config list | awk '/default/{print $2}')"

cat > "${SOURCE_PATH}/.clangd" <<EOF
CompileFlags:
  CompilationDatabase: ${OUTPUT_PATH}
EOF
 
echo "Created ${SOURCE_PATH}/.clangd pointing to ${OUTPUT_PATH}"
