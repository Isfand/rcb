PORTNAME=       rcb
DISTVERSION=    0.12.3
CATEGORIES=     sysutils

MAINTAINER=     ports@FreeBSD.org
COMMENT=        File recycling utility for the terminal
WWW=            https://github.com/Isfand/rcb

LICENSE=        EPL-2.0
LICENSE_NAME=   Eclipse Public License 2.0
LICENSE_FILE=   ${WRKSRC}/LICENSE.txt
LICENSE_PERMS=  dist-mirror dist-sell pkg-mirror pkg-sell auto-accept

BUILD_DEPENDS=  build2>=0:devel/build2
LIB_DEPENDS=    libsqlite3.so:databases/sqlite3

USES=           compiler:c++23-lang

USE_GITHUB=     yes
GH_ACCOUNT=     Isfand
GH_PROJECT=     rcb
GH_TAGNAME=     v${DISTVERSION}

B_CMD=          ${LOCALBASE}/bin/b
B_ARGS=         -V -j ${MAKE_JOBS_NUMBER} -J ${MAKE_JOBS_NUMBER}

do-build:
	@cd ${WRKSRC}/rcb && ${SETENV} ${MAKE_ENV} ${B_CMD} configure ${B_ARGS} \
		config.cxx=${CXX} \
		config.cxx.coptions="${CXXFLAGS}" \
		config.cxx.loptions="${LDFLAGS} ${STRIP}" \
		config.bin.lib=shared \
		config.bin.rpath=${PREFIX}/lib \
		config.install.chroot=${STAGEDIR} \
		config.install.root=${PREFIX}

do-install:
	@cd ${WRKSRC} && ${SETENV} ${MAKE_ENV} ${B_CMD} install ${B_ARGS}

.include <bsd.port.mk>