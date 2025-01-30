PORTNAME=       rcb
PORTVERSION=    0.10.0
CATEGORIES=     sysutils
MASTER_SITES=   https://github.com/Isfand/rcb/archive/refs/tags/
DISTFILES=      v0.10.0.tar.gz
WWW=            https://github.com/Isfand/rcb

MAINTAINER=     ports@FreeBSD.org
COMMENT=        File trashing utility for the terminal

LICENCE=        EPLv2
LICENSE_NAME=   Eclipse Public License 2.0
LICENSE_FILE=	${WRKSRC}/LICENSE.txt

USES=           compiler:c++23-lang

BUILD_DEPENDS=  build2>=0:devel/build2
LIB_DEPENDS=	libsqlite3.so:databases/sqlite3

B_CMD=		${LOCALBASE}/bin/b
B_ARGS=		-V -j ${MAKE_JOBS_NUMBER} -J ${MAKE_JOBS_NUMBER}

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
	@cd ${WRKSRC} && ${SETENV} ${MAKE_ENV} ${B_CMD} install ${B_ARGS}

.include <bsd.port.mk>