#!/bin/sh

#After Usage: Manually refresh/restart the language server.

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SOURCE_DIR="$(cd "$SCRIPT_DIR" && b info | awk '/^src_root:/ {print $2}')"
OUTPUT_DIR="$(cd "$SCRIPT_DIR" && bdep config list | awk '/default/{print $2}')"

cat > "${SOURCE_DIR}/.clangd" <<EOF
CompileFlags:
  CompilationDatabase: ${OUTPUT_DIR}
EOF
 
echo "Created ${SOURCE_DIR}/.clangd pointing to ${OUTPUT_DIR}"
