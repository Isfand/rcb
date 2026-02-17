#pragma once
#ifndef UTILS_HXX
#define UTILS_HXX

#include <filesystem>
#include <string>
#include <print>

namespace rcb{

/* contains common shared program utilities */

//This exists because it does two actions and by default directory_entry follows symbolic links. And I don't want todo std::filesystem::exists(entry.symlink_status) every single time.
struct Verity
{
	std::filesystem::file_type type{};
	bool exists{};

	Verity(const std::filesystem::directory_entry& entry);
};

std::string fileTypeToString(std::filesystem::file_type ft);

void externRename(const std::filesystem::path& filePath, const std::filesystem::path& newCopy);
bool canMvFileChk(const std::filesystem::directory_entry& file);
bool canReadDirChk(const std::filesystem::directory_entry& entry);
std::array<std::array<bool, 3>, 3> getFilePerms(const std::filesystem::path& file);
unsigned long long directorySize(const std::filesystem::directory_entry& directory);
int formatToTimestamp(const std::string& format, long long& timestamp);

//Unused
std::string posixTimeToDateTime(std::chrono::seconds timestamp);

//Unimplemented
std::string dataUnitConversion();

void renameFile(std::string& file);
bool renameDupe(
	const std::filesystem::path& directory, 
	std::filesystem::directory_entry stagePath,
	std::string& mutFilename);

std::filesystem::path deepestExistingPath(const std::filesystem::path& fullPath);
int sanitizeRemoveAll(const std::filesystem::path& path);

//Add a thread pool here

} //namespace rcb

#endif //UTILS_HXX
