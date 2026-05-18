#if defined(_WIN32) && !defined(__CYGWIN__)

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif

#include <windows.h>
#include <aclapi.h>   // GetNamedSecurityInfoW, SE_FILE_OBJECT
#include <io.h>       // _get_osfhandle
#include <cerrno>
#include <expected>
#include <filesystem>
#include <memory>
#include <string_view>

#include "faccessat_win32.hxx"

// ── Implementation details ────────────────────────────────────────────────────
namespace {

[[nodiscard]] int win_error_to_errno(DWORD err) noexcept
{
	switch (err)
	{
		case ERROR_FILE_NOT_FOUND:
		case ERROR_PATH_NOT_FOUND:
		case ERROR_INVALID_DRIVE:
		case ERROR_BAD_NETPATH:             return ENOENT;
		case ERROR_ACCESS_DENIED:
		case ERROR_SHARING_VIOLATION:
		case ERROR_LOCK_VIOLATION:          return EACCES;
		case ERROR_INVALID_HANDLE:          return EBADF;
		case ERROR_NOT_ENOUGH_MEMORY:
		case ERROR_OUTOFMEMORY:             return ENOMEM;
		case ERROR_INVALID_PARAMETER:       return EINVAL;
		case ERROR_FILENAME_EXCED_RANGE:
		case ERROR_META_EXPANSION_TOO_LONG: return ENAMETOOLONG;
		case ERROR_DIRECTORY:               return ENOTDIR;
		case ERROR_PRIVILEGE_NOT_HELD:      return EPERM;
		case ERROR_TOO_MANY_OPEN_FILES:     return EMFILE;
		default:                            return EIO;
	}
}

// ─────────────────────────────────────────────────────────────────────────────

struct HandleDeleter {
	void operator()(HANDLE h) const noexcept {
		if (h && h != INVALID_HANDLE_VALUE) CloseHandle(h);
	}
};
using UniqueHandle = std::unique_ptr<std::remove_pointer_t<HANDLE>, HandleDeleter>;

struct LocalDeleter {
	void operator()(void* p) const noexcept { if (p) LocalFree(p); }
};
using UniqueSD = std::unique_ptr<void, LocalDeleter>;

// ─────────────────────────────────────────────────────────────────────────────

[[nodiscard]] std::expected<std::filesystem::path, int>
path_from_handle(HANDLE h)
{
	std::wstring buf(32'768, L'\0');
	const DWORD len = GetFinalPathNameByHandleW(
		h, buf.data(), static_cast<DWORD>(buf.size()),
		FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);

	if (len == 0)
		return std::unexpected{win_error_to_errno(GetLastError())};

	buf.resize(len);
	return std::filesystem::path{std::move(buf)};
}

[[nodiscard]] std::expected<std::filesystem::path, int>
resolve_path(int dirfd, const char* pathname)
{
	namespace fs = std::filesystem;
	fs::path rel{pathname};

	if (rel.is_absolute()) return rel;

	if (dirfd == AT_FDCWD) {
		std::error_code ec;
		auto cwd = fs::current_path(ec);
		if (ec) return std::unexpected{ec.value()};
		return cwd / rel;
	}

	const HANDLE h = reinterpret_cast<HANDLE>(_get_osfhandle(dirfd));
	if (h == INVALID_HANDLE_VALUE)
		return std::unexpected{EBADF};

	return path_from_handle(h).transform([&](const fs::path& base) {
		return base / rel;
	});
}

// ─────────────────────────────────────────────────────────────────────────────

// X_OK on a file: check executable extension (mirrors _wstat64 behaviour).
[[nodiscard]] bool is_executable_ext(const std::filesystem::path& path) noexcept
{
	std::wstring ext = path.extension().wstring();
	for (auto& c : ext) c = towupper(c);

	using namespace std::string_view_literals;
	constexpr std::wstring_view kExts[] = {
		L".EXE"sv, L".COM"sv, L".BAT"sv, L".CMD"sv
	};
	for (auto e : kExts)
		if (ext == e) return true;
	return false;
}

[[nodiscard]] std::expected<void, int>
check_access(const std::filesystem::path& path, int mode, bool follow_symlinks)
{
	// F_OK — existence only, no access rights needed.
	if (mode == F_OK) {
		const DWORD attrs = GetFileAttributesW(path.c_str());
		if (attrs == INVALID_FILE_ATTRIBUTES)
			return std::unexpected{win_error_to_errno(GetLastError())};
		return {};
	}

	const DWORD attrs = GetFileAttributesW(path.c_str());
	if (attrs == INVALID_FILE_ATTRIBUTES)
		return std::unexpected{win_error_to_errno(GetLastError())};

	const bool is_dir = (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;

	// X_OK on a plain file: extension-based heuristic.
	if ((mode & X_OK) && !is_dir && !is_executable_ext(path))
		return std::unexpected{EACCES};

	// ── Build the desired access mask ─────────────────────────────────────────
	DWORD desired = 0;
	if (mode & R_OK)
		desired |= is_dir ? FILE_LIST_DIRECTORY    : FILE_GENERIC_READ;
	if (mode & W_OK)
		desired |= is_dir ? (FILE_ADD_FILE | FILE_ADD_SUBDIRECTORY | FILE_DELETE_CHILD)
		                  : FILE_GENERIC_WRITE;
	// X_OK on a directory = traverse permission.
	if ((mode & X_OK) && is_dir)
		desired |= FILE_TRAVERSE;

	if (desired == 0)
		return {};

	// ── Fetch security descriptor by path (no handle needed) ─────────────────
	// GetNamedSecurityInfoW avoids opening the file with READ_CONTROL, which
	// would fail for paths where the process only has traverse permission.
	// AT_SYMLINK_NOFOLLOW: append a trailing backslash to prevent auto-follow
	// on symlinks — GetNamedSecurityInfoW follows by default.
	std::wstring wpath = follow_symlinks
		? path.wstring()
		: path.wstring();

	// For AT_SYMLINK_NOFOLLOW, open the reparse point directly to get its
	// security descriptor via a handle instead of the path-based API.
	PSECURITY_DESCRIPTOR pRawSD{};
	if (!follow_symlinks) {
		UniqueHandle hReparse{CreateFileW(
			path.c_str(), READ_CONTROL,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			nullptr, OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
			nullptr)};

		if (!hReparse || hReparse.get() == INVALID_HANDLE_VALUE)
			return std::unexpected{win_error_to_errno(GetLastError())};

		if (const DWORD rc = GetSecurityInfo(
				hReparse.get(), SE_FILE_OBJECT,
				OWNER_SECURITY_INFORMATION |
				GROUP_SECURITY_INFORMATION |
				DACL_SECURITY_INFORMATION,
				nullptr, nullptr, nullptr, nullptr, &pRawSD);
			rc != ERROR_SUCCESS)
			return std::unexpected{win_error_to_errno(rc)};
	} else {
		if (const DWORD rc = GetNamedSecurityInfoW(
				wpath.data(), SE_FILE_OBJECT,
				OWNER_SECURITY_INFORMATION |
				GROUP_SECURITY_INFORMATION |
				DACL_SECURITY_INFORMATION,
				nullptr, nullptr, nullptr, nullptr, &pRawSD);
			rc != ERROR_SUCCESS)
			return std::unexpected{win_error_to_errno(rc)};
	}

	UniqueSD pSD{pRawSD};

	// ── Duplicate process token for AccessCheck ───────────────────────────────
	HANDLE hRawToken{};
	if (!OpenProcessToken(GetCurrentProcess(),
			TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_IMPERSONATE,
			&hRawToken))
		return std::unexpected{win_error_to_errno(GetLastError())};

	UniqueHandle hToken{hRawToken};

	HANDLE hRawImp{};
	if (!DuplicateToken(hToken.get(), SecurityImpersonation, &hRawImp))
		return std::unexpected{win_error_to_errno(GetLastError())};

	UniqueHandle hImp{hRawImp};

	// ── Run the ACL-based access check ────────────────────────────────────────
	GENERIC_MAPPING mapping{
		FILE_GENERIC_READ,
		FILE_GENERIC_WRITE,
		FILE_GENERIC_EXECUTE,
		FILE_ALL_ACCESS};

	MapGenericMask(&desired, &mapping);

	PRIVILEGE_SET privSet{};
	DWORD         privSetLen   = sizeof(privSet);
	DWORD         grantedAccess{};
	BOOL          accessStatus{FALSE};

	if (!AccessCheck(
			pSD.get(), hImp.get(), desired, &mapping,
			&privSet, &privSetLen, &grantedAccess, &accessStatus))
		return std::unexpected{win_error_to_errno(GetLastError())};

	if (!accessStatus)
		return std::unexpected{EACCES};

	return {};
}

} // namespace

// ── aci::Faccessat::Faccessat ─────────────────────────────────────────────────

namespace aci {

Faccessat::Faccessat(int dirfd, const char* pathname, int mode, int flags) noexcept
{
	if (!pathname) { errno = EFAULT; result_ = -1; return; }

	constexpr int kValidMode  = F_OK | R_OK | W_OK | X_OK;
	constexpr int kValidFlags = AT_EACCESS | AT_SYMLINK_NOFOLLOW;

	if (mode  & ~kValidMode)  { errno = EINVAL; result_ = -1; return; }
	if (flags & ~kValidFlags) { errno = EINVAL; result_ = -1; return; }

	// AT_EACCESS — no-op (no real vs. effective UID split on Windows).
	const bool follow_symlinks = !(flags & AT_SYMLINK_NOFOLLOW);

	auto path = resolve_path(dirfd, pathname);
	if (!path) { errno = path.error(); result_ = -1; return; }

	auto res = check_access(*path, mode, follow_symlinks);
	if (!res)  { errno = res.error();  result_ = -1; return; }

	result_ = 0;
}

} // namespace aci

#endif //#if defined(_WIN32) && !defined(__CYGWIN__)