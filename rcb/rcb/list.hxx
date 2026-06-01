#pragma once

#include <string>
#include <vector>
#include <format>

#include "common/globals.hxx"
#include "common/database.hxx"

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
	bool previousOption{};

	bool pastOption{};
	std::vector<std::string> timeVec{};

	bool sqlOption{};
	std::vector<std::string> sqlVec{};
};

class List
{
public:
	List(const ListOptions& lOpt);
private:
	const ListOptions& m_lOpt;
	Database m_db;
	std::vector<std::string> m_validFiles{};
	std::string m_defaultSQLQuery{std::format("SELECT * FROM {0}", DTO::Meta::kTableName)};

	void allFile();
	void file(const std::vector<std::string>& args);
	void count();
	void size();
	void past();
	void previous();
	void sqlInjection();
};
	
} // namespace rcb
