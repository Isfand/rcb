#include <array>
#include <format>
#include <print>
#include <stdexcept>

#include <sqlite3.h>

#include "database.hxx"
#include "globals.hxx"

namespace trash{
   
void Database::createDB()
{
	sqlite3_open((singleton->getWorkingTrashDataDir() / m_dataBaseName).string().c_str(), &m_db);
	sqlite3_close(m_db);
}

void Database::createTable()
{
	sqlite3_open((singleton->getWorkingTrashDataDir() / m_dataBaseName).string().c_str(), &m_db);

	std::string defaultSQLTable = std::format("CREATE TABLE IF NOT EXISTS \"{0}\" (\n"
											  "\t\"{1}\" INTEGER PRIMARY KEY AUTOINCREMENT,\n"
											  "\t\"{2}\" varchar(65535) UNIQUE,\n"
											  "\t\"{3}\" varchar(65535),\n"
											  "\t\"{4}\" BIGINT UNSIGNED,\n"
											  "\t\"{5}\" BIGINT UNSIGNED,\n" 
											  "\t\"{6}\" varchar(65535),\n"
											  "\t\"{7}\" varchar(65535),\n" 
											  "\t\"{8}\" BIGINT UNSIGNED);\n",
											  g_progName, 
											  m_fileIdColumn, 
											  m_fileNameColumn, 
											  m_filePathColumn, 
											  m_fileTimestampColumn, 
											  m_fileSizeColumn,
											  m_fileTypeColumn,
											  m_fileUserColumn,
											  m_fileExecutionIDColumn);
											  //sqlite3 does not support UNSIGNED. It is simply ignored. Keep it here for a workaround.

	char* errorMsg{};
	int exit {sqlite3_exec(m_db, defaultSQLTable.c_str(), NULL, 0, &errorMsg)};

	if (exit != SQLITE_OK)
	{
		sqlite3_free(&errorMsg);
			
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
		
	sqlite3_close(m_db);

} 

void Database::insertData(const std::array<std::string, 7>& fileDetails)
{
	char* errorMsg{};

	sqlite3_open((singleton->getWorkingTrashDataDir() / m_dataBaseName).string().c_str(), &m_db);

	//TODO. Add g_progname as interpolation for 'trash'
	std::string recordData = std::format("INSERT INTO {} ("
										 "{}, {}, {}, {}, {}, {}, {}) "
										 "VALUES("
										 "'{}', '{}', '{}', '{}', '{}', '{}', '{}');",
										 g_progName, 
										 m_fileNameColumn,
										 m_filePathColumn,
										 m_fileTimestampColumn,
										 m_fileSizeColumn,
										 m_fileTypeColumn,
										 m_fileUserColumn,
										 m_fileExecutionIDColumn,
										 fileDetails.at(0),
										 fileDetails.at(1),
										 fileDetails.at(2),
										 fileDetails.at(3),
										 fileDetails.at(4),
										 fileDetails.at(5),
										 fileDetails.at(6));
	
	int exit = {sqlite3_exec(m_db, recordData.c_str(), NULL, 0, &errorMsg)};
	if (exit != SQLITE_OK)
	{
		sqlite3_free(&errorMsg);
			
#ifndef NDEBUG
			std::println("Failed to add record");
			throw std::invalid_argument("insertData() Failed");
#endif
	}
	else 
	{
#ifndef NDEBUG
			std::println("Success from default SQL table data insert"); // WILL always return even if exists
#endif       
	}
	
	sqlite3_close(m_db);
}

//For delete.cxx & wipe.cxx
std::string Database::selectData(const std::string& sql)
{
	sqlite3_stmt *stmt;
	int rc;
	
	std::string st{};

	rc = sqlite3_open((singleton->getWorkingTrashDataDir() / m_dataBaseName).string().c_str(), &m_db);
	if (rc != SQLITE_OK) 
	{
		throw std::runtime_error(std::format("Cannot open database: {}\n", sqlite3_errmsg(m_db)));
	}

	rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK) 
	{
		sqlite3_close(m_db);
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
			st.append(col_text);
		}
	}

	if (rc != SQLITE_DONE)
	{
		throw std::runtime_error(std::format("Failed to execute statement: {}\n", sqlite3_errmsg(m_db)));
	}

	sqlite3_finalize(stmt);
	sqlite3_close(m_db);

	return st;
 
}

//For list.cxx
//TODO: Would be better to have this function return a type of std::vector<std::vector<std::string>> or custom struct instead of a plain string.
std::string Database::selectDataA(const std::string& sql)
{
	sqlite3_stmt *stmt;

	int rc;
	std::string st{};

	rc = sqlite3_open((singleton->getWorkingTrashDataDir() / m_dataBaseName).string().c_str(), &m_db);
	if (rc != SQLITE_OK) 
	{
		throw std::runtime_error(std::format("Cannot open database: {}\n", sqlite3_errmsg(m_db)));
	}

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
	sqlite3_close(m_db);
	return st;
}

//For Restore.cxx & Wipe.cxx(allFile())
//Can be overloaded instead of having a unique function name using discriminatory tokens (enums)
std::vector<std::string> Database::selectDataB(const std::string& sql)
{
	std::vector<std::string> vlist{};
	sqlite3_stmt *stmt;
	int rc;

	rc = sqlite3_open((singleton->getWorkingTrashDataDir() / m_dataBaseName).string().c_str(), &m_db);
	if (rc != SQLITE_OK) 
	{
		throw std::runtime_error(std::format("Cannot open database: {}\n", sqlite3_errmsg(m_db)));
	}

	rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK) 
	{
		sqlite3_close(m_db);
		throw std::runtime_error(std::format("Failed to prepare statement: {}\n", sqlite3_errmsg(m_db)));
	}

	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
	{
		int col_count = sqlite3_column_count(stmt);
		for (int col = 0; col < col_count; col++) 
		{
			//const char* col_name = sqlite3_column_name(stmt, col);
			const char* col_text = (const char*)sqlite3_column_text(stmt, col);
			// printf("%s: %s\n", col_name, col_text ? col_text : "NULL");
			vlist.push_back(col_text);
		}
	}

	if (rc != SQLITE_DONE) 
	{
		throw std::runtime_error(std::format("Failed to execute statement: {}\n", sqlite3_errmsg(m_db)));
	}

	sqlite3_finalize(stmt);
	sqlite3_close(m_db);

	return vlist;
}

int Database::executeSQL(const std::string &sql)
{
	char *errMsg = nullptr;
	int rc;

	rc = sqlite3_open((singleton->getWorkingTrashDataDir() / m_dataBaseName).string().c_str(), &m_db);
	if (rc != SQLITE_OK) 
	{
		throw std::runtime_error(std::format("Cannot open database: {}\n", sqlite3_errmsg(m_db)));
	}

	rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
	if (rc != SQLITE_OK) 
	{
		sqlite3_free(errMsg); // Free the error message memory
		sqlite3_close(m_db);  // Close the database connection

		throw std::runtime_error(std::format("SQL error: {}\n", errMsg));
	}

	sqlite3_close(m_db);

	return SQLITE_OK;
}

} //namespace trash
