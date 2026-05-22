#if defined(_WIN32) && !defined(__CYGWIN__)
#pragma once

//Provisional implementation

/**
 * faccessat_win32.hxx
 * --------------------
 * Windows drop-in for the POSIX faccessat(2) syscall.
 *
 * Requires:   C++23  (/std:c++latest or -std=c++23)
 * Link with:  kernel32.lib  (default in MSVC/MinGW)
 *
 * Usage:
 *   Faccessat f(AT_FDCWD, "config.ini", R_OK, 0);
 *   if (f.result() == 0) { ... }
 */

// ── POSIX constants ───────────────────────────────────────────────────────────
#ifndef F_OK
#  define F_OK 0
#endif
#ifndef X_OK
#  define X_OK 1
#endif
#ifndef W_OK
#  define W_OK 2
#endif
#ifndef R_OK
#  define R_OK 4
#endif
#ifndef AT_FDCWD
#  define AT_FDCWD            (-100)
#endif
#ifndef AT_EACCESS
#  define AT_EACCESS          0x200
#endif
#ifndef AT_SYMLINK_NOFOLLOW
#  define AT_SYMLINK_NOFOLLOW 0x100
#endif

// ─────────────────────────────────────────────────────────────────────────────

namespace aci {

class Faccessat
{
public:
	/**
	 * @param dirfd     Open directory fd, or AT_FDCWD.
	 * @param pathname  Path — absolute, or relative to dirfd / CWD.
	 * @param mode      F_OK, or any OR-combination of R_OK | W_OK | X_OK.
	 * @param flags     0, AT_EACCESS, AT_SYMLINK_NOFOLLOW, or both.
	 */
	Faccessat(int dirfd, const char* pathname, int mode, int flags) noexcept;

	/**
	 * @return  0 on success, -1 on error with errno set.
	 */
	[[nodiscard]] int result() const noexcept { return result_; }

private:
	int result_;
};

} // namespace aci

#endif //#if defined(_WIN32) && !defined(__CYGWIN__)