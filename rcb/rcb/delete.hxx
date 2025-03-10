#pragma once
#ifndef DELETE_HXX
#define DELETE_HXX

#include <string>
#include <array>
#include <filesystem>
#include <vector>

#include "delete_args.hxx"
#include "common/database.hxx"

namespace rcb{

class Delete
{
public:
	Delete(const std::vector<std::string>& args, const DeleteOptions& dOpt);
private:
	const DeleteOptions& m_dOpt;
	Database m_db;
	std::string m_currentExecutionID;
	
	void file(const std::vector<std::string>& args);
	const std::array<std::string, 8> saveFileData(const std::string& stageFilename, const std::filesystem::path& originalDir);
	unsigned long long fileSize(const std::filesystem::directory_entry& file);
	std::string incrementExecutionID();

	bool hasTrailingSlash(const std::filesystem::path& path);
	std::filesystem::path removeTrailingSlash(const std::filesystem::path& path);
	int pathDepth(const std::filesystem::path& path);
};
	
}//namespace rcb

#endif
