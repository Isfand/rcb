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

Erase::Erase(const std::vector<std::string>& args, const EraseOptions& eOpt) : m_eOpt{eOpt}
{
#ifndef NDEBUG
	std::println("allOption is:      {}", m_eOpt.allOption);
	std::println("pastOption is:     {}", m_eOpt.pastOption);
	std::println("previousOption is: {}", m_eOpt.previousOption);
	std::println("verboseOption is:  {}", m_eOpt.verboseOption); //Unused
	std::println("silentOption is:   {}", m_eOpt.silentOption);
	std::println("sqlOption is:      {}", m_eOpt.sqlOption);
#endif
	Erase::file(args);
	if(m_eOpt.allOption)     Erase::allFile();
	if(m_eOpt.sqlOption)     Erase::sqlInjection();
	if(m_eOpt.pastOption)    Erase::past();
	if(m_eOpt.previousOption)Erase::previous();
}

void Erase::file(const std::vector<std::string>& args)
{
	for(const std::string& arg : args)
	{
		//TODO, Add a check for "" empty. Then continue the loop.
		std::string stagedFile = m_db.selectData(std::format("SELECT {0} FROM {1} WHERE {2}='{3}';", 
			g_kSchemaFile, g_kProgName, g_kSchemaID, arg));

		try 
		{
			std::filesystem::rename(g_singleton->getWorkingProgFileDir() / stagedFile, g_singleton->getWorkingProgWipeDir() / stagedFile);
			sanitizeRemoveAll(g_singleton->getWorkingProgWipeDir() / stagedFile);
		}
		catch (std::filesystem::filesystem_error& e) 
		{
			if(!m_eOpt.silentOption) std::cerr << e.what() << "\n";
			continue;
		}
		//Once removed then delete from database
		m_db.executeSQL(std::format("DELETE FROM {0} WHERE {1}='{2}';", 
			g_kProgName, g_kSchemaID, arg));
	}
}


void Erase::allFile()
{
	std::vector<std::string> vList { m_db.selectDataB(std::format("SELECT {0} FROM {1};", 
		g_kSchemaID, g_kProgName)) };

#ifndef NDEBUG
	std::println("Printing all existing record IDs");
	for(auto str : vList)
	{
		std::println("{0}", str);
	}
#endif
	Erase::file(vList);
}

//TODO: past() is very similar across erase, list, restore. Find a way to share past()
void Erase::past()
{
	//TODO. add silence to guard the return cerr text
	for(auto format : m_eOpt.timeVec)
	{
		long long timestamp{};
		int return_code = formatToTimestamp(format, timestamp);
		
		if (return_code == 0)
		{
			std::vector<std::string> vList { m_db.selectDataB(std::format("SELECT {0} FROM {1} WHERE {2} >= {3};", 
				g_kSchemaID, g_kProgName, g_kSchemaTimestamp, timestamp)) };
			Erase::file(vList);
		}
		else if(return_code == 1 && !m_eOpt.silentOption) std::cerr << "error: units not found\n";
		else if(return_code == 2 && !m_eOpt.silentOption) std::cerr << "error: invalid format " << format << "\n";
		else if(return_code == 3 && !m_eOpt.silentOption) std::cerr << "unexpected failure\n";
	}
}

void Erase::previous()
{
	std::vector<std::string> vList { m_db.selectDataB(std::format("SELECT {0} FROM {2} WHERE {1}=(SELECT MAX({1}) FROM {2});", 
		g_kSchemaID, g_kSchemaExecution, g_kProgName)) };
	Erase::file(vList);
}

//Must return id. E.G "SELECT id FROM rcb WHERE file='.hiddenfile';" 
void Erase::sqlInjection()
{
	for(auto sql : m_eOpt.sqlVec)
	{
#ifndef NDEBUG
		std::println("SQL Statement is: {}", sql);
#endif
		std::vector<std::string> vList { m_db.selectDataB(sql) };
		Erase::file(vList);
	}
}

//MOVE all files to wipe/ These will now be marked for being permanently erased as with anything inside wipe/
//Have the rcb file names be saved in RAM. And check against it before deleting
//When CONFIRMED. Then delete them. unlink()
//Then finally DELETE from database
	
}//namespace rcb
