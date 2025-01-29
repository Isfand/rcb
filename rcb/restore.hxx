#pragma once
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
	Restore(std::vector<std::string>& args, RestoreOptions& rOpt);

private:
	RestoreOptions& m_rOpt;
	
	void file(std::vector<std::string>& args);

	void allFile();
	bool checkTrashFile(const std::string& trashFile);
	bool checkOriginalPath(const std::filesystem::path& trashDir);
	void past();
	void previous();
	void sqlInjection();
};

} // rcb namespace

#endif // RESTORE_HXX
