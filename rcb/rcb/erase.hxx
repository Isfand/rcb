#pragma once

#include <string>
#include <vector>

#include "common/database.hxx"
#include "common/env.hxx"

namespace rcb{

struct EraseOptions
{
	bool allOption{};
	bool lastOption{};
	bool verboseOption{};
	bool dryRunOption{};
	
	bool pastOption{};
	std::vector<std::string> timeVec{};

	bool sqlOption{};
	std::vector<std::string> sqlVec{};
};

class Erase
{
public:
	Erase(const std::vector<std::string>& args, const EraseOptions& eOpt, const Env& env);
private:
	const EraseOptions& m_eOpt;
	const Env& m_env;
	Database m_db;

	void file(const std::vector<std::string>& args);
	void allFile();
	void past();
	void last();
	void sqlInjection();
};
	
} // namespace rcb
