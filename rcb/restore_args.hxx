#pragma once
#ifndef RESTORE_ARGS_HXX
#define RESTORE_ARGS_HXX

#include <string>
#include <vector>

namespace rcb{

struct RestoreOptions
{
	//bool defaultOption{};
	bool allOption{};
	bool verboseOption{};
	bool forceOption{};
	bool forceReplaceOption{};
	bool forceRenameOption{};
	bool silentOption{};
	bool previousOption{};

	bool pastOption{};
	std::vector<std::string> timeVec{};
	
	bool sqlOption{};
	std::vector<std::string> sqlVec{};
};

class RestoreArgs
{
public:
	RestoreArgs(std::vector<std::string>& args);
	void run(std::vector<std::string>& args);
private:
	std::vector<int> m_erasePositions{};
	RestoreOptions m_rOpt;
	
	const char* m_restoreHelpMsg = 
		"restore usage:\n" 
		"restore <n> | r <n>, restore file by id.\n"
		"--help      | -h\n"
		"--verbose   | -v\n"
		"--all       | -a\n"
		"--force-replace\n"
		"--force-rename\n"
		"--past <n>(unit), t,s,m,h,d,w,mo,y,c,k,ae\n"
		"--previous\n"
		"--sql <s>, UNSAFE: Insert sql statement directly for restore. Must return ID.\n"
		;
};

} // namespace rcb

#endif // RESTORE_ARGS_HXX
