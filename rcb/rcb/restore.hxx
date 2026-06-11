#pragma once

#include <string>
#include <filesystem>
#include <vector>

#include "common/database.hxx"
#include "common/env.hxx"

namespace rcb{

struct RestoreOptions
{
	// bool defaultOption{};
	bool allOption{};
	bool verboseOption{};
	bool forceOption{};
	bool forceReplaceOption{};
	bool forceRenameOption{};
	bool forceRecreateDirectoryOption{};
	bool lastOption{};
	bool dryRunOption{};

	bool pastOption{};
	std::vector<std::string> timeVec{};
	
	bool sqlOption{};
	std::vector<std::string> sqlVec{};
};

class Restore
{
public:
	Restore(const std::vector<std::string>& args, const RestoreOptions& rOpt, const Env& env);

private:
	enum class PathStatus
	{
		Occupied,
		Free,
		NoParent
	};

	const RestoreOptions& m_rOpt;
	const Env& m_env;
	Database m_db;
	
	void file(const std::vector<std::string>& args);

	void allFile();
	bool progFileExists(const std::string& stagedFile);
	PathStatus originalPathStatus(const std::filesystem::path& progDir);
	void past();
	void last();
	void sqlInjection();
};

} // rcb namespace