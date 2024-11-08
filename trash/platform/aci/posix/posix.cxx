#if defined(__unix__) || defined(__APPLE__)
#include <stdexcept>
#include <cstring>
#include <vector>

#include <pwd.h>

#include <utime.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

// For GNU libc & musl this provides the proper macros.
#if __has_include(<sys/sysmacros.h>)
#include <sys/sysmacros.h>
#endif

#include "posix.hxx"

namespace aci{

//NOTE: FreeBSD's aliases can be unsigned long long instead of linux's unsigned int.
//TODO: Add overloads for std::string for all. Or use a template.
//TODO: For readability create new aliases that resemble the posix ones as close as possible.

Stat::Stat(const char* filePath)
{
	//stat will follow symlinks. lstat will not.
	if (lstat(filePath, &m_stat) == -1)
		throw std::runtime_error("Error getting file information");
}

unsigned long long Stat::st_atim()    const
{
	//TODO. Implement
	return 0;
};      
unsigned long long Stat::st_blksize() const
{
	return m_stat.st_blksize;
};      
unsigned long long Stat::st_blocks()  const
{
	return m_stat.st_blocks;
};      
unsigned long long Stat::st_ctim()    const
{
	//TODO. Implement
	return 0;
};
unsigned long long Stat::st_dev() const
{
	unsigned long long device = m_stat.st_dev;
	return device;
};
unsigned long long Stat::st_dev_major() const
{
	//XXX. fix or investigate why the minor number is different on FreeBSD when using stat from cli compared to the one reported here.
	unsigned long long majorNum = major(m_stat.st_dev);

	// Convert to string and format as "major,minor"
	// std::string deviceIdStr = std::to_string(majorNum) + "," + std::to_string(minorNum);

	return majorNum;
};
unsigned long long Stat::st_dev_minor() const
{
	unsigned long long minorNum = minor(m_stat.st_dev);
	return minorNum;
}
unsigned long long Stat::st_gid()     const
{
	return m_stat.st_gid;
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
	//TODO. Implement
	return 0;
};
unsigned long long Stat::st_nlink()   const
{
	return m_stat.st_nlink;
};
unsigned long long Stat::st_rdev()    const
{
	return m_stat.st_rdev;
};
unsigned long long Stat::st_size()    const
{
	return m_stat.st_size;
};
unsigned long long Stat::st_uid()     const
{
	return m_stat.st_uid;
};

Chown::Chown(const char* path, unsigned long long new_uid, unsigned long long new_gid)
{
	if (change_owner(path, new_uid, new_gid) == -1ULL) throw std::runtime_error("Failed to change owner");
};
unsigned long long Chown::change_owner(const char* path, unsigned long long new_uid, unsigned long long new_gid)
{
	return chown(path, new_uid, new_gid);
}

Utime::Utime(const char* path, long long new_atime, long long new_mtime)
{
	if (change_times(path, new_atime, new_mtime) == -1ULL) throw std::runtime_error("Failed to change time");
}
unsigned long long Utime::change_times(const char* path, long long new_atime, long long new_mtime)
{
	struct utimbuf new_times;
	new_times.actime = new_atime;   // Access time
	new_times.modtime = new_mtime;  // Modification time
	return utime(path, &new_times);
}

//Utimensat::Utimensat(const char* path, struct timespec new_atime, struct timespec new_mtime)
//{
//	if (change_times_ns(path, new_atime, new_mtime) == -1) throw std::runtime_error("Failed to change time in nanoseconds");
//}
//unsigned long long Utimensat::change_times_ns(const char* path, struct timespec new_atime, struct timespec new_mtime)
//{
//	struct timespec times[2];
//	times[0] = new_atime;  // Access time
//	times[1] = new_mtime;  // Modification time
//	return utimensat(AT_FDCWD, path, times, 0);
//}

unsigned long long User::uid()
{
	return getuid();
}
unsigned long long User::groupuid()
{
	return getgid();
}
std::vector<unsigned long long> User::groups()
{
	int ngroups = getgroups(0, nullptr);  // Get the number of groups first

	if (ngroups == -1) 
		throw std::runtime_error("Error retrieving the number of groups.");
	// Use a vector to dynamically allocate memory for the group IDs
	std::vector<gid_t> groups_gid_t(ngroups);

	// Now get the actual group IDs
	int num_groups = getgroups(ngroups, groups_gid_t.data());  // Pass pointer to vector's data
	if (num_groups == -1) 
		throw std::runtime_error("Error retrieving group IDs."); 
	std::vector<unsigned long long> groups(groups_gid_t.begin(), groups_gid_t.end());
	return groups;
}

Pwuid::Pwuid(unsigned long long id) : m_id{id}
{
	m_pwuid = getpwuid(m_id);
}
std::string Pwuid::pw_dir() const
{
	return m_pwuid->pw_dir;
}
std::string Pwuid::pw_gecos() const
{
	return m_pwuid->pw_gecos;
}
unsigned long long Pwuid::pw_gid() const
{
	return m_pwuid->pw_gid;
}
std::string Pwuid::pw_name() const
{
	return m_pwuid->pw_name;
}
std::string Pwuid::pw_passwd() const
{
	return m_pwuid->pw_passwd;
}
std::string Pwuid::pw_shell() const
{
	return m_pwuid->pw_shell;
}
unsigned long long Pwuid::pw_uid() const
{
	return m_pwuid->pw_uid;
}

Pwnam::Pwnam(const char* name) : m_name{name}
{
	m_pwnam = getpwnam(m_name);
}
std::string Pwnam::pw_dir() const
{
	return m_pwnam->pw_dir;
}
std::string Pwnam::pw_gecos() const
{
	return m_pwnam->pw_gecos;
}
unsigned long long Pwnam::pw_gid() const
{
	return m_pwnam->pw_gid;
}
std::string Pwnam::pw_name() const
{
	return m_pwnam->pw_name;
}
std::string Pwnam::pw_passwd() const
{
	return m_pwnam->pw_passwd;
}
std::string Pwnam::pw_shell() const
{
	return m_pwnam->pw_shell;
}
unsigned long long Pwnam::pw_uid() const
{
	return m_pwnam->pw_uid;
}

}//namespace aci

#endif //#if defined(__unix__) || defined(__APPLE__)