PORTNAME=       trash
PORTVERSION=    0.9.4
CATEGORIES=     sysutils
MASTER_SITES=   https://github.com/Isfand/trash

#MAINTAINER=     you@example.com
COMMENT=        File trashing utility for the terminal

LICENSE=        Eclipse Public License 2.0

USES=           build2

# Set build2-specific arguments
BUILD_DEPENDS=  build2:devel/build2 \
				sqlite3:databases/sqlite3

do-build:
    cd ${WRKSRC} && bdep init -C @x64-posix-llvm-release cc config.config.load=tools/build-configs/posix-llvm-release.config && bdep sync "?sys:libsqlite3"

do-install:
    cd ${WRKSRC} && b install config.cxx.loptions="-L/usr/local/lib -Wl,-rpath,/usr/local/lib" config.install.root="/usr/local" config.install.sudo="sudo"

# Metadata
#PLIST_FILES=    bin/trash
