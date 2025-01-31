#pragma once
#ifndef DATABASE_HXX
#define DATABASE_HXX

#include <string>
#include <vector>
#include <format>

#include <sqlite3.h>

#include "globals.hxx"

namespace rcb{

class Database
{
public:
	void createDB();
	void createTable();
	void insertData(const std::array<std::string, 8>& fileDetails);
	std::string selectData(const std::string& sql);
	std::string selectDataA(const std::string& sql);
	std::vector<std::string> selectDataB(const std::string& sql);
	int executeSQL(const std::string &sql);
private:
	sqlite3* m_db;                                          //Shared sqlite3 instance
	std::string m_dataBaseName           {std::format("{}.sqlite3", g_progName)}; //{"rcb.sqlite3"}; //Database filename

	const char* m_fileIdColumn           {"id"};            //SQL auto increment
	const char* m_fileNameColumn         {"file"};          //Refers to filename to be inside of .rcb/file/
	const char* m_filePathColumn         {"path"};          //Complete filesystem path
	const char* m_fileTimestampColumn    {"timestamp"};     //Unix Timestamp since epoch
	const char* m_fileSizeColumn         {"size"};          //Bytes
	const char* m_fileTypeColumn         {"filetype"};      //Filesystem file type
	const char* m_filePathDepthColumn    {"depth"};         //Path depth
	const char* m_fileUserColumn         {"user"};          //The user who executed the command
	const char* m_fileExecutionIDColumn  {"execution"};     //Execution id of all files deleted in the same run
};

} //namespace rcb

#endif //DATABASE_HXX
