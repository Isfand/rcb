#pragma once
#ifndef ERASE_HXX
#define ERASE_HXX

#include <string>
#include <vector>

#include "erase_args.hxx"
#include "common/database.hxx"

namespace rcb{

class Erase
{
public:
	Erase(const std::vector<std::string>& args, const EraseOptions& eOpt);
private:
	const EraseOptions& m_eOpt;
	Database m_db;

	void file(const std::vector<std::string>& args);
	void allFile();
	void past();
	void previous();
	void sqlInjection();
};
	
} //namespace rcb

#endif //ERASE_HXX 
