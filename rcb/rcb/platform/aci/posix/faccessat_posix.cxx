#if defined(__unix__) || defined(__APPLE__)

#include "faccessat_posix.hxx"

// ── aci::Faccessat::Faccessat ─────────────────────────────────────────────────

namespace aci {

Faccessat::Faccessat(int dirfd, const char* pathname, int mode, int flags) noexcept
{
	result_ = faccessat(dirfd, pathname, mode, flags);
}

} // namespace aci

#endif // #if defined(__unix__) || defined(__APPLE__)
