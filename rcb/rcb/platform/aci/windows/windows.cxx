#if defined(_WIN32) && !defined(__CYGWIN__)

#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <print>
#include <sstream>
#include <iomanip>

#include <sys/stat.h> //Needed for mingw
#include <windows.h>
#include <lmcons.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <sddl.h>

#include "windows.hxx"

namespace aci{

Stat::Stat(const char* filePath)
{
	//HANDLE handleFile = CreateFileA(
	//	filePath,
	//	GENERIC_READ,
	//	FILE_SHARE_READ,
	//	NULL,
	//	OPEN_EXISTING,
	//	FILE_ATTRIBUTE_NORMAL,
	//	NULL
	//);

	//GetFileAttributesExA(filePath, GetFileExInfoStandard, &m_fileAttributeInfo);
	//GetFileInformationByHandle(handleFile, &m_fileHandleInfo);

	//Use _stat instead as it doesn't follow symbolic links by default on windows.
	//Update: It does follow symlinks but only by checking if the target path exists. If not -1 is returned. Needs new implementation.

	if (_stat(filePath, &m_stat) == -1)
		throw std::runtime_error("Error getting file information");
}

unsigned long long Stat::st_atim()    const
{
	return 0;
};
unsigned long long Stat::st_blksize() const
{
	return 0;
};
unsigned long long Stat::st_blocks()  const
{
	return 0;
};
unsigned long long Stat::st_ctim()    const
{
	return 0;
};
unsigned long long Stat::st_dev() const
{
	unsigned long long device = m_stat.st_dev;
	return device;
};
unsigned long long Stat::st_dev_major() const
{
	//No Major macro on windows.
	unsigned long long majorNum = (m_stat.st_dev);
	return majorNum;
};
unsigned long long Stat::st_dev_minor() const
{
	//No Minor macro on windows.
	unsigned long long minorNum = (m_stat.st_dev);
	return minorNum;
}
unsigned long long Stat::st_gid()     const
{
	return 0;
};
unsigned long long Stat::st_ino()     const
{
	return m_stat.st_ino;
};
unsigned long long Stat::st_mode()    const
{
	return m_stat.st_mode;
};
unsigned long long Stat::st_mtim()    const
{
	return 0;
};
unsigned long long Stat::st_nlink()   const
{
	return 0;
};
unsigned long long Stat::st_rdev()    const
{
	return 0;
};
unsigned long long Stat::st_size()    const
{
	return 0;
};
unsigned long long Stat::st_uid()     const
{
	return 0;
};

Chown::Chown(const char* path, unsigned long long new_uid, unsigned long long new_gid)
{

};
unsigned long long Chown::change_owner(const char* path, unsigned long long new_uid, unsigned long long new_gid)
{
	return 0;
}

Utime::Utime(const char* path, long long new_atime, long long new_mtime)
{

}
unsigned long long Utime::change_times(const char* path, long long new_atime, long long new_mtime)
{
	return 0;
}

User::User()
{
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) 
		std::runtime_error("Failed to open process token. Error: " + std::to_string(GetLastError()));

	DWORD dwSize = 0;
	GetTokenInformation(hToken, TokenUser, nullptr, 0, &dwSize);
	if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) 
	{
		CloseHandle(hToken);
		std::runtime_error("Failed to get token information size. Error: " + std::to_string(GetLastError()));
	}

	std::unique_ptr<BYTE[]> pTokenInfo(new BYTE[dwSize]);
	if (!GetTokenInformation(hToken, TokenUser, pTokenInfo.get(), dwSize, &dwSize)) 
	{
		CloseHandle(hToken);
		std::runtime_error("Failed to get token information. Error: " + std::to_string(GetLastError()));
	}

	// Extract SID
	TOKEN_USER* pTokenUser = reinterpret_cast<TOKEN_USER*>(pTokenInfo.get());
	LPSTR sidString;
	if (!ConvertSidToStringSidA(pTokenUser->User.Sid, &sidString)) 
	{
		CloseHandle(hToken);
		std::runtime_error("Failed to convert SID to string. Error: " + std::to_string(GetLastError()));
	}

	std::string sidStringResult(sidString);
	LocalFree(sidString);
	CloseHandle(hToken);

	m_sidStringResult = sidStringResult;
}
unsigned long long User::uid() const
{
	//Seed the SID to have a unique ID.
	//This formula can potentially create collisions. Unlikely as a normal machine only has like 5-30 SIDs.
	unsigned long long seed{};
	const unsigned long long prime{ 31 }; // A small prime number for hashing

	for (char c : m_sidStringResult) 
		seed = seed * prime + static_cast<unsigned long long>(c);

	return seed;
}
unsigned long long User::groupuid()
{
	return 0;
}
std::vector<unsigned long long> User::groups()
{
	std::vector<unsigned long long> s {};
	return s;
}

Pwuid::Pwuid(unsigned long long id) : m_id{id}
{

}
std::string Pwuid::pw_dir() const
{
	WCHAR* path = nullptr;  // Pointer to WCHAR, which will be allocated by SHGetKnownFolderPath

	// KnownFolderID for the user's profile folder
	GUID folderId = FOLDERID_Profile;

	// Retrieve the user's home directory path
	if (SUCCEEDED(SHGetKnownFolderPath(folderId, 0, NULL, &path)))
	{
		// Convert WCHAR (UTF-16) string to a narrow multi-byte string (e.g., UTF-8)
		int bufferSize = WideCharToMultiByte(CP_UTF8, 0, path, -1, nullptr, 0, nullptr, nullptr);
		if (bufferSize > 0)
		{
			std::string homeDir(bufferSize - 1, 0);  // Subtract 1 to avoid adding extra null terminator
			WideCharToMultiByte(CP_UTF8, 0, path, -1, &homeDir[0], bufferSize, nullptr, nullptr);

			// Free the memory allocated for path by SHGetKnownFolderPath
			CoTaskMemFree(path);

			return homeDir;
		}
	}
	// Return an empty string if the function fails
	return "";
}
std::string Pwuid::pw_gecos() const
{
	return "";
}
unsigned long long Pwuid::pw_gid() const
{
	return 0;
}
//XXX: Broken buffer contents. Figure out why a pre-fixed ? question mark (0x00 I.E a null-terminator) is added to the name.
std::string Pwuid::pw_name() const
{
	WCHAR username[UNLEN + 1]; // UNLEN is the max username length
	DWORD username_len = UNLEN + 1;

	// Get the username
	if (!GetUserNameW(username, &username_len))
	{
		return ""; 
	}

	// Convert from UTF-16 to UTF-8 using WideCharToMultiByte
	int buffer_size = WideCharToMultiByte(CP_UTF8, 0, username, -1, nullptr, 0, nullptr, nullptr);
	if (buffer_size == 0)
	{
		return "";
	}

	std::string str(buffer_size, 0);
	WideCharToMultiByte(CP_UTF8, 0, username, -1, &str[0], buffer_size, nullptr, nullptr);

	return str;

	// Convert from wstring (UTF-16) to string (UTF-8)
	//std::wstring wstr(username);
	//std::string str(wstr.begin(), wstr.end());
}
std::string Pwuid::pw_passwd() const
{
	return "";
}
std::string Pwuid::pw_shell() const
{
	return "";
}
unsigned long long Pwuid::pw_uid() const
{
	return 0;
}

Pwnam::Pwnam(const char* name) : m_name{name}
{

}
std::string Pwnam::pw_dir() const
{
	return "";
}
std::string Pwnam::pw_gecos() const
{
	return "";
}
unsigned long long Pwnam::pw_gid() const
{
	return 0;
}
std::string Pwnam::pw_name() const
{
	return "";
}
std::string Pwnam::pw_passwd() const
{
	return "";
}
std::string Pwnam::pw_shell() const
{
	return "";
}
unsigned long long Pwnam::pw_uid() const
{
	return 0;
}

}//namespace aci

#endif //#if defined(_WIN32) && !defined(__CYGWIN__)