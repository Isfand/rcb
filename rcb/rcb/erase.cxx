#include <format>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <print>

#include "erase.hxx"
#include "common/database.hxx"
#include "common/globals.hxx"
#include "common/utils.hxx"

namespace rcb{

Erase::Erase(const std::vector<std::string>& args, const EraseOptions& eOpt) : m_eOpt{eOpt}, m_db(g_singleton->getWorkingProgDataDir() / DTO::Meta::kDatabaseName)
{
#ifndef NDEBUG
	std::println("allOption is:      {}", m_eOpt.allOption);
	std::println("pastOption is:     {}", m_eOpt.pastOption);
	std::println("lastOption is:     {}", m_eOpt.lastOption);
	std::println("verboseOption is:  {}", m_eOpt.verboseOption); // Unused
	std::println("dryRunOption is:   {}", m_eOpt.dryRunOption); // Unused
	std::println("sqlOption is:      {}", m_eOpt.sqlOption);
#endif
	Erase::file(args);
	if(m_eOpt.allOption)     Erase::allFile();
	if(m_eOpt.sqlOption)     Erase::sqlInjection();
	if(m_eOpt.pastOption)    Erase::past();
	if(m_eOpt.lastOption)    Erase::last();
}

void Erase::file(const std::vector<std::string>& args)
{
	for(const std::string& arg : args)
	{
		// TODO, Add a check for "" empty. Then continue the loop.
		std::string stagedFile = m_db.selectValue(std::format("SELECT {0} FROM {1} WHERE {2}='{3}';", 
			DTO::Meta::kSchemaFile, DTO::Meta::kTableName, DTO::Meta::kSchemaID, arg));

		try 
		{
			if(!m_eOpt.dryRunOption)
			{
				std::filesystem::rename(g_singleton->getWorkingProgFileDir() / stagedFile, g_singleton->getWorkingProgWipeDir() / stagedFile);
				sanitizeRemoveAll(g_singleton->getWorkingProgWipeDir() / stagedFile);
				m_db.executeSQL(std::format("DELETE FROM {0} WHERE {1}='{2}';", DTO::Meta::kTableName, DTO::Meta::kSchemaID, arg)); // Once removed then delete from database
			}
		}
		catch (std::filesystem::filesystem_error& e) 
		{
			std::cerr << e.what() << "\n";
			continue;
		}
	}
}


void Erase::allFile()
{
	std::vector<std::string> vList { m_db.selectColumn(std::format("SELECT {0} FROM {1};", 
		DTO::Meta::kSchemaID, DTO::Meta::kTableName)) };

#ifndef NDEBUG
	std::println("Printing all existing record IDs");
	for(auto str : vList)
	{
		std::println("{0}", str);
	}
#endif
	Erase::file(vList);
}

// TODO: past() is the same across erase, list, restore. Reduce to one.
void Erase::past()
{
	// TODO. add silence to guard the return cerr text
	for(auto format : m_eOpt.timeVec)
	{
		long long timestamp{};
		int return_code = formatToTimestamp(format, timestamp);
		
		if (return_code == 0)
		{
			std::vector<std::string> vList { m_db.selectColumn(std::format("SELECT {0} FROM {1} WHERE {2} >= {3};", 
				DTO::Meta::kSchemaID, DTO::Meta::kTableName, DTO::Meta::kSchemaTimestamp, timestamp)) };
			Erase::file(vList);
		}
		else if(return_code == 1) std::cerr << "error: units not found\n";
		else if(return_code == 2) std::cerr << "error: invalid format " << format << "\n";
		else if(return_code == 3) std::cerr << "unexpected failure\n";
	}
}

void Erase::last()
{
	std::vector<std::string> vList { m_db.selectColumn(std::format("SELECT {0} FROM {2} WHERE {1}=(SELECT MAX({1}) FROM {2});", 
		DTO::Meta::kSchemaID, DTO::Meta::kSchemaBatch, DTO::Meta::kTableName)) };
	Erase::file(vList);
}

// Must return id. E.G "SELECT id FROM rcb WHERE file='.hiddenfile';" 
void Erase::sqlInjection()
{
	for(auto sql : m_eOpt.sqlVec)
	{
#ifndef NDEBUG
		std::println("SQL Statement is: {}", sql);
#endif
		std::vector<std::string> vList { m_db.selectColumn(sql) };
		Erase::file(vList);
	}
}

// MOVE all files to wipe/ These will now be marked for being permanently erased as with anything inside wipe/
// Have the rcb file names be saved in RAM. And check against it before deleting
// When CONFIRMED. Then delete them. unlink()
// Then finally DELETE from database
	
}// namespace rcb
