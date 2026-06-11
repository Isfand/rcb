#pragma once

#include <string>
#include <filesystem>
#include <vector>

#include "common/database.hxx"
#include "common/env.hxx"

namespace rcb{

struct DeleteOptions
{
	bool verboseOption{};
	bool forceOption{};
	bool noDirSizeOption{};
	bool dryRunOption{};
};

class Delete
{
public:
	Delete(const std::vector<std::string>& args, const DeleteOptions& dOpt, const Env& env);
private:
	const DeleteOptions& m_dOpt;
	const Env& m_env;
	Database m_db;
	unsigned long long m_currentExecutionID;
	
	void file(const std::vector<std::string>& args);
	const DTO saveFileData(const std::string& stageFilename, const std::filesystem::path& originalDir);
	unsigned long long fileSize(const std::filesystem::directory_entry& file);
	unsigned long long incrementBatchID();

	bool hasTrailingSlash(const std::filesystem::path& path);
	std::filesystem::path removeTrailingSlash(const std::filesystem::path& path);
	unsigned long long pathDepth(const std::filesystem::path& path);
};
	
}// namespace rcb
