#pragma once

#include <string>
#include <vector>
#include <format>

#include "common/database.hxx"
#include "common/env.hxx"

namespace rcb{

struct ListOptions
{
	bool allOption{};
	bool defaultOption{};
	bool totalSizeOption{};
	bool totalCountOption{};
	bool humanReadableOption{};
	bool noFormatOption{};
	bool verboseOption{};
	bool lastOption{};

	bool pastOption{};
	std::vector<std::string> timeVec{};

	bool sqlOption{};
	std::vector<std::string> sqlVec{};
};

class List
{
public:
	List(const ListOptions& lOpt, const Env& env);
private:
	const ListOptions& m_lOpt;
	const Env& m_env;
	Database m_db;
	std::vector<std::string> m_validFiles{};
	std::string m_defaultSQLQuery{std::format("SELECT * FROM {0}", DTO::Meta::kTableName)};

	void allFile();
	void file(const std::vector<std::string>& args);
	void count();
	void size();
	void past();
	void last();
	void sqlInjection();
};
	
} // namespace rcb
