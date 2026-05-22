#pragma once
#ifndef LIST_ARGS_HXX
#define LIST_ARGS_HXX

#include <string>
#include <vector>

#include "list.hxx"

namespace rcb{

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
		"--past <n>(unit), t,s,m,h,d,w,mo,y,c,k\n"
		"--previous\n"
		"--total-count, list total count of all files\n"
		"--total-size, list total size of all files\n"
		"--no-format, remove formatting\n"
		"--sql <s>, UNSAFE: Insert sql statement directly for list\n"
		;

};
	
} // namespace rcb

#endif // LIST_ARGS_HXX 
