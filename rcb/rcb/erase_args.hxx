#pragma once
#ifndef ERASE_ARGS_HXX
#define ERASE_ARGS_HXX

#include <string>
#include <vector>

namespace rcb{

struct EraseOptions
{
	bool allOption{};
	bool previousOption{};
	bool verboseOption{};
	bool silentOption{};

	bool pastOption{};
	std::vector<std::string> timeVec{};

	bool sqlOption{};
	std::vector<std::string> sqlVec{};
};
	
class EraseArgs
{
public:
	EraseArgs(std::vector<std::string>& args);
	void run(std::vector<std::string>& args);
private:
	EraseOptions m_eOpt;
	std::vector<int> m_erasePositions{};

	const char* m_eraseHelpMsg = 
		"erase usage:\n" 
		"erase <n> | e <n>, erase file by id.\n"
		"--help    | -h \n"
		"--all     | -a, permanently remove all files\n"
		"--past <n>(unit), t,s,m,h,d,w,mo,y,c,k,ae\n"
		"--previous\n"
		"--sql <s>, UNSAFE: Insert sql statement directly for erase. Must return ID.";
};

	
} //namespace rcb

#endif //ERASE_ARGS_HXX 
