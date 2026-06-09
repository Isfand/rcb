#pragma once

#include <string>
#include <vector>
#include <optional>
#include <filesystem>

#include <sqlite3.h>

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
		static constexpr const char* kSchemaBatch     {"batch"};       // batch id of all files deleted in the same run	
	};

	std::optional<unsigned long long>    id;
	std::optional<std::string>           file;
	std::optional<std::filesystem::path> path;
	std::optional<long long>             timestamp;
	std::optional<unsigned long long>    size;
	std::optional<std::string>           filetype;
	std::optional<unsigned long long>    depth;
	std::optional<std::string>           user;
	std::optional<unsigned long long>    batch;
};

inline std::string nullableInt (const auto& opt){return opt ? std::to_string(*opt) : "NULL";}
inline std::string nullableStr (const std::optional<std::string>& opt){return opt.value_or("NULL");}
inline std::string nullablePath(const std::optional<std::filesystem::path>& opt){return opt ? opt->string() : "NULL";}

class Database
{
public:
	Database(const std::filesystem::path& location);
	~Database();

	Database(const Database&) = delete; // delete copy
	Database& operator=(const Database&) = delete; // delete copy

	void createDB();
	void resetCounter();
	void createTable();
	std::string selectValue(const std::string& sql);
	std::string selectDisplay(const std::string& sql);
	std::vector<std::string> selectColumn(const std::string& sql);
	int executeSQL(const std::string &sql);

	//NOTE: DTO datatypes are being narrowed/expanded or converted when being inserted/retrieved
	void insertDTO(const DTO& fileDetails);
	std::vector<DTO> selectDTO(const std::string& sql);
private:
	sqlite3* m_db = nullptr; // Shared sqlite3 instance
};

} // namespace rcb