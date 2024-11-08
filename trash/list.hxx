#pragma once
#ifndef LIST_HXX
#define LIST_HXX

#include <string>
#include <vector>

#include "list_args.hxx"
#include "common/database.hxx"

namespace trash{

class List
{
public:
	List(ListOptions& lOpt);
private:
	ListOptions& m_lOpt;
	Database m_db;
	std::vector<std::string> m_validFiles{};

	void allFile();
	void file(std::vector<std::string>& args);
	void count();
	void size();
	void past();
	void previous();
	void sqlInjection();
};
	
} //namespace trash

#endif //LIST_ARGS_HXX 
