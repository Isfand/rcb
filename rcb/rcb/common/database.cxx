#include <format>
#include <print>
#include <stdexcept>
#include <unordered_map>

#include <sqlite3.h>

#include "database.hxx"

namespace rcb{

Database::Database(const std::filesystem::path& location)
{
	int rc = sqlite3_open(location.string().c_str(), &m_db);
	if (rc != SQLITE_OK)
		throw std::runtime_error(std::format("Cannot open database: {}\n", sqlite3_errmsg(m_db)));
}
Database::~Database()
{
	if (m_db) sqlite3_close(m_db);
}

// Resets counter if no records exist.
// Note: can be replaced with executeSQL() internally
void Database::resetCounter()
{
	std::string query = std::format("DELETE FROM sqlite_sequence WHERE name = '{0}' AND NOT EXISTS ( SELECT 1 FROM {0} LIMIT 1);", DTO::Meta::kTableName);

	char* errorMsg{};
	int exit {sqlite3_exec(m_db, query.c_str(), NULL, 0, &errorMsg)};

	if (exit != SQLITE_OK)
	{
		sqlite3_free(errorMsg);
			
#ifndef NDEBUG
		std::println("Failed to reset sequence counter");
#endif
		throw std::invalid_argument("resetCounter() Failed");
	}
	else
	{
#ifndef NDEBUG
		std::println("Success from resetCounter()");
#endif
	}
}

// Note: can be replaced with executeSQL() internally
void Database::createTable()
{
	std::string defaultSQLTable = std::format("CREATE TABLE IF NOT EXISTS \"{0}\" (\n"
											  "\t\"{1}\" INTEGER PRIMARY KEY AUTOINCREMENT,\n"
											  "\t\"{2}\" varchar(65535) UNIQUE,\n"
											  "\t\"{3}\" varchar(65535),\n"
											  "\t\"{4}\" BIGINT,\n"
											  "\t\"{5}\" BIGINT UNSIGNED,\n" 
											  "\t\"{6}\" varchar(65535),\n"
											  "\t\"{7}\" BIGINT UNSIGNED,\n" 
											  "\t\"{8}\" varchar(65535),\n" 
											  "\t\"{9}\" BIGINT UNSIGNED);\n",
											  DTO::Meta::kTableName, 
											  DTO::Meta::kSchemaID, 
											  DTO::Meta::kSchemaFile, 
											  DTO::Meta::kSchemaPath, 
											  DTO::Meta::kSchemaTimestamp, 
											  DTO::Meta::kSchemaSize,
											  DTO::Meta::kSchemaFiletype,
											  DTO::Meta::kSchemaPathDepth,
											  DTO::Meta::kSchemaUser,
											  DTO::Meta::kSchemaBatch);
											  // sqlite3 does not support UNSIGNED. It is simply ignored. Keep it here for a workaround.

	char* errorMsg{};
	int exit {sqlite3_exec(m_db, defaultSQLTable.c_str(), NULL, 0, &errorMsg)};

	if (exit != SQLITE_OK)
	{
		sqlite3_free(errorMsg);
			
#ifndef NDEBUG
		std::println("Failed to create default database table");
#endif
		throw std::invalid_argument("createTable() Failed");
	}
	else
	{
#ifndef NDEBUG
		std::println("Success from default SQL database table creation"); // WILL always return even if exists
#endif
	}
} 

// For delete.cxx & wipe.cxx
// Scalar
std::string Database::selectValue(const std::string& sql)
{
	sqlite3_stmt *stmt;
	int rc;
	
	std::string st{};

	rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK) 
	{
		throw std::runtime_error(std::format("Failed to prepare statement: {}\n", sqlite3_errmsg(m_db)));
	}

	// Execute the SQL statement and retrieve the results
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
	{
		int col_count = sqlite3_column_count(stmt);
		for (int col = 0; col < col_count; col++) 
		{
			// const char* col_name = sqlite3_column_name(stmt, col);
			const char* col_text = (const char*)sqlite3_column_text(stmt, col);
			if (col_text) st.append(col_text);
		}
	}

	if (rc != SQLITE_DONE)
	{
		throw std::runtime_error(std::format("Failed to execute statement: {}\n", sqlite3_errmsg(m_db)));
	}

	sqlite3_finalize(stmt);
	return st;
 
}

// For list.cxx
// Formatted string for printing 
std::string Database::selectDisplay(const std::string& sql)
{
	sqlite3_stmt *stmt;

	int rc;
	std::string st{};

	rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK) 
	{
		throw std::runtime_error(std::format("Failed to prepare statement: {}\n", sqlite3_errmsg(m_db)));
	}

	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
	{
		int col_count = sqlite3_column_count(stmt);
		for (int col = 0; col < col_count; col++) 
		{
			const char* col_name = sqlite3_column_name(stmt, col);
			const char* col_text = (const char*)sqlite3_column_text(stmt, col);
			// printf("%s: %s\n", col_name, col_text ? col_text : "NULL");
			st.append(col_name);
			st.append(":");
			st.append(col_text ? col_text : "NULL");
			if ((col + 1) != col_count) { st.append(" | "); }
		}
			st.append("\n");
	}

	if (rc != SQLITE_DONE)
	{
		throw std::runtime_error(std::format("Failed to execute statement: {}\n", sqlite3_errmsg(m_db)));
	}

	sqlite3_finalize(stmt);
	return st;
}

// For Restore.cxx & Wipe.cxx(allFile())
// Vector of single-column values
std::vector<std::string> Database::selectColumn(const std::string& sql)
{
	std::vector<std::string> vlist{};
	sqlite3_stmt *stmt;
	int rc;

	rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK) 
	{
		throw std::runtime_error(std::format("Failed to prepare statement: {}\n", sqlite3_errmsg(m_db)));
	}

	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
	{
		int col_count = sqlite3_column_count(stmt);
		for (int col = 0; col < col_count; col++) 
		{
			// const char* col_name = sqlite3_column_name(stmt, col);
			const char* col_text = (const char*)sqlite3_column_text(stmt, col);
			// printf("%s: %s\n", col_name, col_text ? col_text : "NULL");
			vlist.push_back(col_text ? col_text : "");
		}
	}

	if (rc != SQLITE_DONE) 
	{
		throw std::runtime_error(std::format("Failed to execute statement: {}\n", sqlite3_errmsg(m_db)));
	}

	sqlite3_finalize(stmt);
	return vlist;
}

// Fire-and-forget (INSERT, UPDATE, DELETE, CREATE)
int Database::executeSQL(const std::string &sql)
{
	char *errMsg = nullptr;
	int rc;

	rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
	if (rc != SQLITE_OK) 
	{
		std::string sqliteErrorMsg = errMsg;
		sqlite3_free(errMsg); // Free the error message memory
		throw std::runtime_error(std::format("SQL error: {}\n", sqliteErrorMsg));
	}

	return SQLITE_OK;
}

void Database::insertDTO(const DTO& fileDetails)
{
	char* queryStr = sqlite3_mprintf(
	"INSERT INTO %w (%w, %w, %w, %w, %w, %w, %w, %w) VALUES(?, ?, ?, ?, ?, ?, ?, ?);",
	DTO::Meta::kTableName,
	DTO::Meta::kSchemaFile,
	DTO::Meta::kSchemaPath,
	DTO::Meta::kSchemaTimestamp,
	DTO::Meta::kSchemaSize,
	DTO::Meta::kSchemaFiletype,
	DTO::Meta::kSchemaPathDepth,
	DTO::Meta::kSchemaUser,
	DTO::Meta::kSchemaBatch);

	sqlite3_stmt* stmt{};
	int rc = sqlite3_prepare_v2(m_db, queryStr, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
#ifndef NDEBUG
		std::println("Failed to prepare statement. sqlite3_prepare_v2() returned error code: {} with error: {}", rc, sqlite3_errmsg(m_db));
#endif
		throw std::invalid_argument("insertData() Failed: could not prepare statement");
	}

	// Helper to bind a string or NULL if the optional is empty
	const auto bindText = [&](int col, const std::optional<std::string>& val) {
		if (val)
			return sqlite3_bind_text(stmt, col, val->c_str(), -1, SQLITE_TRANSIENT);
		return sqlite3_bind_null(stmt, col);
	};

	const std::string pathStr = fileDetails.path ? fileDetails.path->string() : "";

	bindText(1, fileDetails.file);
	fileDetails.path
		? sqlite3_bind_text(stmt, 2, pathStr.c_str(), -1, SQLITE_TRANSIENT)
		: sqlite3_bind_null(stmt, 2);
	fileDetails.timestamp
		? sqlite3_bind_int64(stmt, 3, *fileDetails.timestamp)
		: sqlite3_bind_null(stmt, 3);
	fileDetails.size
		? sqlite3_bind_int64(stmt, 4, static_cast<sqlite3_int64>(*fileDetails.size))
		: sqlite3_bind_null(stmt, 4);
	bindText(5, fileDetails.filetype);
	fileDetails.depth
		? sqlite3_bind_int64(stmt, 6, static_cast<sqlite3_int64>(*fileDetails.depth))
		: sqlite3_bind_null(stmt, 6);
	bindText(7, fileDetails.user);
	fileDetails.batch
		? sqlite3_bind_int64(stmt, 8, static_cast<sqlite3_int64>(*fileDetails.batch))
		: sqlite3_bind_null(stmt, 8);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
	{
#ifndef NDEBUG
		std::println("Failed to add record. sqlite3_step() returned error code: {} with error: {}", rc, sqlite3_errmsg(m_db));
#endif
		sqlite3_finalize(stmt);
		throw std::invalid_argument("insertData() Failed");
	}

#ifndef NDEBUG
	std::println("Success from default SQL table data insert");
#endif

	sqlite3_finalize(stmt);
}

std::vector<DTO> Database::selectDTO(const std::string& sql)
{
	sqlite3_stmt* stmt{};
	int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);

	if (rc != SQLITE_OK)
	{
#ifndef NDEBUG
		std::println("Failed to prepare statement. sqlite3_prepare_v2() returned error code: {} with error: {}", rc, sqlite3_errmsg(m_db));
#endif
		throw std::invalid_argument("selectDTO() Failed: could not prepare statement");
	}

	// Build a name -> column index map from the result set
	const int colCount = sqlite3_column_count(stmt);
	std::unordered_map<std::string, int> colIndex;
	for (int i = 0; i < colCount; ++i)
		colIndex.emplace(sqlite3_column_name(stmt, i), i);

	const auto getInt64 = [&](const char* name) -> std::optional<long long>
	{
		auto it = colIndex.find(name);
		if (it == colIndex.end()) return std::nullopt;
		if (sqlite3_column_type(stmt, it->second) == SQLITE_NULL) return std::nullopt;
		return sqlite3_column_int64(stmt, it->second);
	};

	const auto getUInt64 = [&](const char* name) -> std::optional<unsigned long long>
	{
		auto it = colIndex.find(name);
		if (it == colIndex.end()) return std::nullopt;
		if (sqlite3_column_type(stmt, it->second) == SQLITE_NULL) return std::nullopt;
		return static_cast<unsigned long long>(sqlite3_column_int64(stmt, it->second));
	};

	const auto getText = [&](const char* name) -> std::optional<std::string>
	{
		auto it = colIndex.find(name);
		if (it == colIndex.end()) return std::nullopt;
		if (sqlite3_column_type(stmt, it->second) == SQLITE_NULL) return std::nullopt;
		return reinterpret_cast<const char*>(sqlite3_column_text(stmt, it->second));
	};

	std::vector<DTO> results;
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		results.push_back(DTO
		{
			.id        = getUInt64(DTO::Meta::kSchemaID),
			.file      = getText  (DTO::Meta::kSchemaFile),
			.path      = getText  (DTO::Meta::kSchemaPath).transform([](const std::string& s) { return std::filesystem::path{s}; }),
			.timestamp = getInt64 (DTO::Meta::kSchemaTimestamp),
			.size      = getUInt64(DTO::Meta::kSchemaSize),
			.filetype  = getText  (DTO::Meta::kSchemaFiletype),
			.depth     = getUInt64(DTO::Meta::kSchemaPathDepth),
			.user      = getText  (DTO::Meta::kSchemaUser),
			.batch     = getUInt64(DTO::Meta::kSchemaBatch)
		});
	}

	if (rc != SQLITE_DONE)
	{
#ifndef NDEBUG
		std::println("selectDTO() sqlite3_step() returned error code: {} with error: {}", rc, sqlite3_errmsg(m_db));
#endif
		sqlite3_finalize(stmt);
		throw std::invalid_argument("selectDTO() Failed");
	}

	sqlite3_finalize(stmt);
	return results;
}

} // namespace rcb
