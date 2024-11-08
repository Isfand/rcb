#pragma once
#ifndef LIST_ARGS_HXX
#define LIST_ARGS_HXX

#include <string>
#include <vector>

namespace trash{

struct ListOptions
{
	bool allOption{};
	bool defaultOption{};
	bool totalSizeOption{};
	bool totalCountOption{};
	bool humanReadableOption{};
	bool noFormatOption{};
	bool verboseOption{};
	bool previousOption{};

	bool pastOption{};
	std::vector<std::string> timeVec{};

	bool sqlOption{};
	std::vector<std::string> sqlVec{};
};

class ListArgs
{
public:
	ListArgs(std::vector<std::string>& args);
	void run(std::vector<std::string>& args);
private:
	ListOptions m_lOpt;
	std::vector<int> m_erasePositions{};
	
	const char* m_listHelpMsg = 
		"list usage:\n" 
		"list      |  l, lists all records by default.\n"
		"--help    | -h \n"
		"--past <n>(unit), t,s,m,h,d,w,mo,y,c,k,ae \n"
		"--previous\n"
		"--total-count, list total count of all files in trash\n"
		"--total-size, list total size of all files in trash\n"
		"--no-format, remove formatting\n"
		"--sql <s>, UNSAFE: Insert sql statement directly for list\n"
		;

};
	
} //namespace trash

#endif //LIST_ARGS_HXX 
