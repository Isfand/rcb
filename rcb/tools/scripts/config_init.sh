#!/bin/sh
 
find_root()
{
	local dir="$SCRIPT_DIR"
	while [ "$dir" != "/" ]; do
		[ -f "$dir/packages.manifest" ] && { echo "$dir"; return 0; }
		dir=$(dirname "$dir")
	done
	echo "ERROR: project root not found" >&2; return 1
}

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
RROOT=$(find_root) || { echo "Aborting"; exit 1; } # Guard prevents continuation
RCB_OUT_DIR="$RROOT/artefacts/rcb"

ARCH=$(uname -m)
UNAME=$(uname)
 
echo $ARCH
 
if [ "$ARCH" = "x86_64" ] || [ "$ARCH" = "amd64" ]; then
	echo "x86_64 Unix"
	sh -c "bdep init -C $RCB_OUT_DIR/x64-posix-llvm-debug @x64-posix-llvm-debug cc config.config.load=$RROOT/rcb/tools/build-configs/posix-llvm-debug.config"
	echo "Done"
	sh -c "bdep init -C $RCB_OUT_DIR/x64-posix-llvm-release @x64-posix-llvm-release cc config.config.load=$RROOT/rcb/tools/build-configs/posix-llvm-release.config"
	echo "Done"
	sh -c "bdep init -C $RCB_OUT_DIR/x64-posix-gcc-debug @x64-posix-gcc-debug cc config.config.load=$RROOT/rcb/tools/build-configs/posix-gcc-debug.config"
	echo "Done"
	sh -c "bdep init -C $RCB_OUT_DIR/x64-posix-gcc-release @x64-posix-gcc-release cc config.config.load=$RROOT/rcb/tools/build-configs/posix-gcc-release.config"
	echo "Done"
	if [ "$UNAME" = "Linux" ]; then
		sh -c "bdep init -C $RCB_OUT_DIR/x64-linux-mingw-release @x64-linux-mingw-release cc config.config.load=$RROOT/rcb/tools/build-configs/linux-mingw-release.config"
		echo "Done"
		sh -c "bdep init -C $RCB_OUT_DIR/x64-posix-muslgcc-release @x64-posix-muslgcc-release cc config.config.load=$RROOT/rcb/tools/build-configs/x86_64-linux-musl-g++-release.config"
		echo "Done"
	fi
fi
 
if [ "$ARCH" = "aarch64" ]; then
	echo "aarch64 Unix"
	sh -c "bdep init -C $RCB_OUT_DIR/arm64-posix-llvm-debug @arm64-posix-llvm-debug cc config.config.load=$RROOT/rcb/tools/build-configs/posix-llvm-debug.config"
	echo "Done"
	sh -c "bdep init -C $RCB_OUT_DIR/arm64-posix-llvm-release @arm64-posix-llvm-release cc config.config.load=$RROOT/rcb/tools/build-configs/posix-llvm-release.config"
	echo "Done"
fi

echo "Exit"

