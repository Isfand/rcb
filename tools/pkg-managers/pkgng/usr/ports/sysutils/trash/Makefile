PORTNAME=       trash
PORTVERSION=    0.9.4
CATEGORIES=     sysutils
MASTER_SITES=   https://github.com/Isfand/trash

#MAINTAINER=     you@example.com
COMMENT=        File trashing utility for the terminal

LICENSE=        Eclipse Public License 2.0
LICENSE_FILE=	${WRKSRC}/LICENSE.txt

USES=           compiler:C++23-lang

# Set build2-specific arguments
BUILD_DEPENDS=  build2>=0:devel/build2
LIB_DEPENDS=	libsqlite3.so:databases/sqlite3

do-build:
	@cd ${WRKSRC} && ${SETENV} ${MAKE_ENV} ${B_CMD} configure ${B_ARGS} \
		config.cxx=${CXX} \
		config.cxx.coptions="${CXXFLAGS}" \
		config.cxx.loptions="${LDFLAGS} ${STRIP}" \
		config.bin.lib=shared \
		config.bin.rpath=${PREFIX}/lib \
		config.install.chroot=${STAGEDIR} \
		config.install.root=${PREFIX}

do-install:
    cd ${WRKSRC} && ${SETENV} ${MAKE_ENV} b install -V -j ${MAKE_JOBS_NUMBER} -J ${MAKE_JOBS_NUMBER}

# Metadata
#PLIST_FILES=    bin/trash
