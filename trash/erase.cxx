#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <filesystem>
#include <print>

#include "erase.hxx"
#include "common/database.hxx"
#include "common/globals.hxx"
#include "common/utils.hxx"

namespace trash{

Erase::Erase(std::vector<std::string>& args, EraseOptions& eOpt) : m_eOpt{eOpt}
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

void Erase::file(std::vector<std::string>& args)
{
	for(std::string& arg : args)
	{
		//TODO, Add a check for "" empty. Then continue the loop.
		//TODO. check perms before rename and remove_all. Show stderr and then continue; 
		std::string trashFile = Database().selectData(std::format("SELECT file FROM trash WHERE id='{0}';", arg));

		try 
		{
			std::filesystem::rename(singleton->getWorkingTrashFileDir() + trashFile, singleton->getWorkingTrashWipeDir() + trashFile);
			std::filesystem::remove_all(singleton->getWorkingTrashWipeDir() + trashFile);
		}
		catch (std::runtime_error& e) 
		{
			//REVISE: 
			/* NOTE: Exception likely occurs due to a permissions issue as remove_all recursively removes files, NOT directories, individually.
			It will only remove a directory when it's empty, otherwise it will continue to remove all contents inside.
			E.G If a directory has another directory that has root ownership with contents inside, they cannot be removed. 
			This is because the program is executed by a user that is missing the required permissions for the nested directory, but has permissions to remove contents in the 
			top-level directory.

			The exception is then thrown. Executing the program as root allows the operation to finish without throwing any exceptions.
			*/
			if(!m_eOpt.silentOption) std::cerr << e.what() << "\n";
			continue;
		}
		//Once removed then delete from database
		Database().executeSQL(std::format("DELETE FROM trash WHERE id='{0}';", arg));
	}
}


void Erase::allFile()
{
	std::vector<std::string> vList { Database().selectDataB("SELECT id FROM trash;") };

#ifndef NDEBUG
	std::println("Printing all existing record IDs");
	for(auto str : vList)
	{
		std::println("{0}", str);
	}
#endif
	Erase::file(vList);
}

void Erase::past()
{
	//TODO. add silence to guard the return cerr text
	for(auto format : m_eOpt.timeVec)
	{
		long long timestamp{};
		int return_code = formatToTimestamp(format, timestamp);
		
		if (return_code == 0)
		{
			std::vector<std::string> vList { Database().selectDataB(std::format("SELECT id FROM trash WHERE timestamp > {};", timestamp)) };
			Erase::file(vList);
		}
		else if(return_code == 1 && !m_eOpt.silentOption) std::cerr << "error: units not found\n";
		else if(return_code == 2 && !m_eOpt.silentOption) std::cerr << "error: invalid format " << format << "\n";
		else if(return_code == 3 && !m_eOpt.silentOption) std::cerr << "unexpected failure\n";
	}
}

void Erase::previous()
{
	std::vector<std::string> vList { Database().selectDataB("SELECT id FROM trash WHERE execution=(SELECT MAX(execution) FROM trash);") };
	Erase::file(vList);
}

//Must return id. E.G "SELECT id FROM trash WHERE file='.hiddenfile';" 
void Erase::sqlInjection()
{
	for(auto sql : m_eOpt.sqlVec)
	{
#ifndef NDEBUG
		std::println("SQL Statement is: {}", sql);
#endif
		std::vector<std::string> vList { Database().selectDataB(sql) };
		Erase::file(vList);
	}
}

//MOVE all files to wipe/ These will now be marked for being permanently erased as with anything inside wipe/
//Have the TRASH file names be saved in RAM. And check against it before deleting
//When CONFIRMED. Then delete them. unlink()
//Then finally DELETE from database
	
}//namespace trash
