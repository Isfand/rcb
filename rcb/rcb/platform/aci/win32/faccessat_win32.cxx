#if defined(_WIN32) && !defined(__CYGWIN__)

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif

#include <windows.h>
#include <io.h>       // _get_osfhandle
#include <cerrno>
#include <expected>
#include <filesystem>
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
check_access(const std::filesystem::path& path, int mode)
{
	const DWORD attrs = GetFileAttributesW(path.c_str());
	if (attrs == INVALID_FILE_ATTRIBUTES)
		return std::unexpected{win_error_to_errno(GetLastError())};

	if (mode == F_OK)
		return {};

	if ((mode & W_OK) && (attrs & FILE_ATTRIBUTE_READONLY))
		return std::unexpected{EACCES};

	if ((mode & X_OK) && !is_executable_ext(path))
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

	// AT_EACCESS          — no-op (no real vs. effective UID split on Windows).
	// AT_SYMLINK_NOFOLLOW — no-op (GetFileAttributesW never follows the leaf).

	auto path = resolve_path(dirfd, pathname);
	if (!path) { errno = path.error(); result_ = -1; return; }

	auto res = check_access(*path, mode);
	if (!res)  { errno = res.error();  result_ = -1; return; }

	result_ = 0;
}

} // namespace aci

#endif //#if defined(_WIN32) && !defined(__CYGWIN__)