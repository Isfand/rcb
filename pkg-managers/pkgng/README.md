Local install instructions for FreeBSD pkgng port.

You may be prompted through an ncursors interface to install dependencies. I.E sqlite3. This can be bypassed using 'make BATCH=yes install clean'

If you're modifying the Makefile beyond a simple value change then it's recommend to use 'portclippy Makefile' afterwards from the portfmt package for validation as it checks Makefile order and variables.

As root:
1. cp -r usr/ports/sysutils/rcb/ /usr/ports/sysutils/
2. cd /usr/ports/sysutils/rcb/

Release build:
1. make rmconfig (Optional)
2. make clean
3. make distclean
4. make makesum
5. make CFLAGS="-O2 -DNDEBUG" LDFLAGS="-s"
6. make install clean

Debug build:
1. make rmconfig (Optional)
2. make clean
3. make distclean
4. make makesum
5. make 
6. make install clean

To uninstall: 
1. make deinstall clean distclean