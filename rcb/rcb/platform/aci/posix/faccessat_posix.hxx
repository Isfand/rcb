#if defined(__unix__) || defined(__APPLE__)
#pragma once

/**
 * faccessat_posix.hxx
 * --------------------
 * Thin POSIX wrapper around faccessat(2).
 *
 * Usage:
 *   aci::Faccessat f(AT_FDCWD, "config.ini", R_OK, 0);
 *   if (f.result() == 0) { ... }
 */

#include <fcntl.h>   // AT_FDCWD, AT_SYMLINK_NOFOLLOW, AT_EACCESS
#include <unistd.h>  // faccessat, F_OK, R_OK, W_OK, X_OK

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

#endif // #if defined(__unix__) || defined(__APPLE__)
