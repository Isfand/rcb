#!/bin/sh
 
ARCH=$(uname -m)
UNAME=$(uname)
 
echo $ARCH
 
if [ "$ARCH" = "x86_64" ] || [ "$ARCH" = "amd64" ]; then
	echo "x86_64 Unix"
	sh -c 'bdep init -C @x64-posix-llvm-debug cc config.config.load=tools/build-configs/posix-llvm-debug.config'
	echo "Done"
	sh -c 'bdep init -C @x64-posix-llvm-release cc config.config.load=tools/build-configs/posix-llvm-release.config'
	echo "Done"
	sh -c 'bdep init -C @x64-posix-gcc-debug cc config.config.load=tools/build-configs/posix-gcc-debug.config'
	echo "Done"
	sh -c 'bdep init -C @x64-posix-gcc-release cc config.config.load=tools/build-configs/posix-gcc-release.config'
	echo "Done"
	if [ "$UNAME" = "Linux" ]; then
		sh -c 'bdep init -C @x64-linux-mingw-release cc config.config.load=tools/build-configs/linux-mingw-release.config' #WIP
		echo "Done"
	fi
	#if [ "$UNAME" = "Linux" ]; then
	#	sh -c 'bdep init -C @arm64-linux-gnueabi-release cc config.config.load=tools/build-configs/linux-gnueabi-release.config'
	#	echo "Done"
	#elif ["$UNAME" = "FreeBSD"]; then
		# Implement
	#fi
	#sh -c 'bdep init -C @x64-mingw-release cc config.config.load=tools/build-configs/linux-mingw-release.config' #WIP
	#echo "Done"
 
fi
 
if [ "$ARCH" = "aarch64" ]; then
	echo "aarch64 Unix"
	sh -c 'bdep init -C @arm64-posix-llvm-debug cc config.config.load=tools/build-configs/posix-llvm-debug.config'
	echo "Done"
	sh -c 'bdep init -C @arm64-posix-llvm-release cc config.config.load=tools/build-configs/posix-llvm-release.config'
	echo "Done"
fi

echo "Exit"

