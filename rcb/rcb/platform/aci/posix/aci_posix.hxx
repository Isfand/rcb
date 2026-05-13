#if defined(__unix__) || defined(__APPLE__)
#pragma once
#ifndef POSIX_HXX
#define POSIX_HXX

#include <vector>
#include <string>

#include <sys/stat.h>
#include <unistd.h>

namespace aci{
   
class Stat
{
public:
	Stat(const char* str);

	unsigned long long st_atim()      const;
	unsigned long long st_blksize()   const;
	unsigned long long st_blocks()    const;
	unsigned long long st_ctim()      const;
	unsigned long long st_dev()       const;
	unsigned long long st_dev_major() const;
	unsigned long long st_dev_minor() const;
	unsigned long long st_gid()       const;
	unsigned long long st_ino()       const;
	unsigned long long st_mode()      const;
	unsigned long long st_mtim()      const;
	unsigned long long st_nlink()     const;
	unsigned long long st_rdev()      const;
	unsigned long long st_size()      const;
	unsigned long long st_uid()       const;
private:
	struct stat m_stat;
};

class Chown
{
public:
	Chown(const char* path, unsigned long long new_uid, unsigned long long new_gid);
private:
	unsigned long long change_owner(const char* path, unsigned long long new_uid, unsigned long long new_gid);
};

class Utime
{
public:
	Utime(const char* path, long long new_atime, long long new_mtime);
private:
	unsigned long long change_times(const char* path, long long new_atime, long long new_mtime);
};

//class Utimensat
//{
//public:
//	Utimensat(const char* path, struct timespec new_atime, struct timespec new_mtime);
//private:
//	unsigned long long change_times_ns(const char* path, struct timespec new_atime, struct timespec new_mtime);
//};

class User
{
public:
	unsigned long long uid();
	unsigned long long groupuid();
	std::vector<unsigned long long> groups();
private:
};

class Pwuid
{
public:
	Pwuid(unsigned long long id = getuid());

	std::string pw_dir() const;
	std::string pw_gecos() const;
	unsigned long long pw_gid() const;
	std::string pw_name() const;
	std::string pw_passwd() const;
	std::string pw_shell() const;
	unsigned long long pw_uid() const;
private:
	unsigned long long m_id;
	struct passwd* m_pwuid;
};

class Pwnam
{
public:
	Pwnam(const char* name);

	std::string pw_dir() const;
	std::string pw_gecos() const;
	unsigned long long pw_gid() const;
	std::string pw_name() const;
	std::string pw_passwd() const;
	std::string pw_shell() const;
	unsigned long long pw_uid() const;
private:
	const char* m_name;
	struct passwd* m_pwnam;
};

}//namespace aci

#endif //UNIX_HXX

#endif //#if defined(__unix__) || defined(__APPLE__)