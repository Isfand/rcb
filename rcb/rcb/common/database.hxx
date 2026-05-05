#pragma once
#ifndef DATABASE_HXX
#define DATABASE_HXX

//TODO: Use the PIMPL design pattern or a forward declaration of struct sqlite3 to hide this header to prevent C pollution. sqlite3* m_db uses it. 
#include <sqlite3.h>

#include <string>
#include <vector>


namespace rcb{

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
	sqlite3* m_db; //Shared sqlite3 instance
};

} //namespace rcb

#endif //DATABASE_HXX
