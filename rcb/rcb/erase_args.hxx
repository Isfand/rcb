#pragma once
#ifndef ERASE_ARGS_HXX
#define ERASE_ARGS_HXX

#include <string>
#include <vector>

#include "erase.hxx"

namespace rcb{
	
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
		"--past <n>(unit), t,s,m,h,d,w,mo,y,dec,c,k\n"
		"--previous\n"
		"--sql <s>, UNSAFE: Insert sql statement directly for erase. Must return ID.";
};

	
} // namespace rcb

#endif // ERASE_ARGS_HXX 
