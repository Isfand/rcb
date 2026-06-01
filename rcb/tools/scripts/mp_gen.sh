#!/bin/sh
#Usage: mp_gen.sh [VERSION]
#Usage: mp_gen.sh
#
#  Generates man pages from asciidoc source files.
#  VERSION overrides the version set in _rcb-shared.adoc.
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SOURCE_DIR="$(cd "$SCRIPT_DIR" && b info | awk '/^src_root:/ {print $2}')"
ADOC_DIR="$SOURCE_DIR/docs/manual/asciidoc"
OUT_DIR="$SOURCE_DIR/docs/manual/man1"
VERSION=${1:-$(git -C "$SOURCE_DIR" describe --tags --abbrev=0 | sed 's/^v//')}
if [ -n "$VERSION" ]; then
    asciidoctor -b manpage -D "$OUT_DIR" -a "rcb-version=$VERSION" $ADOC_DIR/rcb*.adoc
else
    asciidoctor -b manpage -D "$OUT_DIR" $ADOC_DIR/rcb*.adoc
fi