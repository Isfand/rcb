#pragma once
#ifndef ERASE_HXX
#define ERASE_HXX

#include <string>
#include <vector>

#include "common/database.hxx"

namespace rcb{

struct EraseOptions
{
	bool allOption{};
	bool previousOption{};
	bool verboseOption{};
	
	bool pastOption{};
	std::vector<std::string> timeVec{};

	bool sqlOption{};
	std::vector<std::string> sqlVec{};
};

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
	
} // namespace rcb

#endif // ERASE_HXX 
