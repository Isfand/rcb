#pragma once
#ifndef LIST_HXX
#define LIST_HXX

#include <string>
#include <vector>
#include <format>

#include "list_args.hxx"
#include "common/database.hxx"

namespace rcb{

class List
{
public:
	List(ListOptions& lOpt);
private:
	ListOptions& m_lOpt;
	Database m_db;
	std::vector<std::string> m_validFiles{};
	std::string m_defaultSQLQuery{std::format("SELECT * FROM {0}", g_progName)};

	void allFile();
	void file(std::vector<std::string>& args);
	void count();
	void size();
	void past();
	void previous();
	void sqlInjection();
};
	
} //namespace rcb

#endif //LIST_ARGS_HXX 
