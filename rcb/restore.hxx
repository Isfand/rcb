#pragma once
#include "rcb/common/database.hxx"
#ifndef RESTORE_HXX
#define RESTORE_HXX

#include <string>
#include <filesystem>
#include <vector>

#include "restore_args.hxx"

namespace rcb{

class Restore
{
public:
	Restore(const std::vector<std::string>& args, const RestoreOptions& rOpt);

private:
	const RestoreOptions& m_rOpt;
	Database m_db;
	
	void file(const std::vector<std::string>& args);

	void allFile();
	bool checkProgFile(const std::string& stagedFile);
	bool checkOriginalPath(const std::filesystem::path& progDir);
	void past();
	void previous();
	void sqlInjection();
};

} // rcb namespace

#endif // RESTORE_HXX
