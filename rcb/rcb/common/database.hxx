#pragma once

// TODO: Use the PIMPL design pattern or a forward declaration of struct sqlite3 to hide this header to prevent C pollution. sqlite3* m_db uses it.
// Or just wait for C++ Modules to be fully implemented.
#include <sqlite3.h>

#include <string>
#include <vector>
#include <optional>
#include <filesystem>

namespace rcb{

// Data Transfer Object.
struct DTO
{
	struct Meta
	{
		static constexpr const char* kDatabaseName    {"rcb.sqlite3"}; // Database filename
		static constexpr const char* kTableName       {"rcb"};         // table name
		
		static constexpr const char* kSchemaID        {"id"};          // SQL auto increment
		static constexpr const char* kSchemaFile      {"file"};        // Refers to filename to be inside of .rcb/file/
		static constexpr const char* kSchemaPath      {"path"};        // Complete filesystem path
		static constexpr const char* kSchemaTimestamp {"timestamp"};   // Unix Timestamp since epoch
		static constexpr const char* kSchemaSize      {"size"};        // Bytes
		static constexpr const char* kSchemaFiletype  {"filetype"};    // Filesystem file type
		static constexpr const char* kSchemaPathDepth {"depth"};       // Path depth
		static constexpr const char* kSchemaUser      {"user"};        // The user who executed the command
		static constexpr const char* kSchemaExecution {"execution"};   // Execution id of all files deleted in the same run	
	};

	std::optional<unsigned long long int> id;
	std::optional<std::string>            file;
	std::optional<std::filesystem::path>  path;
	std::optional<long long int>          timestamp;
	std::optional<unsigned long long int> size;
	std::optional<std::string>            string;
	std::optional<unsigned long long int> depth;
	std::optional<std::string>            user;
	std::optional<unsigned long long int> execution;
};

class Database
{
public:
	void createDB();
	void resetCounter();
	void createTable();
	void insertData(const std::array<std::string, 8>& fileDetails);
	std::string selectData(const std::string& sql);
	std::string selectDataA(const std::string& sql);
	std::vector<std::string> selectDataB(const std::string& sql);
	int executeSQL(const std::string &sql);
private:
	sqlite3* m_db; // Shared sqlite3 instance
};

} // namespace rcb