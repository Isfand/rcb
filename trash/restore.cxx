#include <filesystem>
#include <iostream>
#include <print>

#include "restore.hxx"
#include "platform/aci/aci.hxx"
#include "common/database.hxx"
#include "common/utils.hxx"
#include "common/globals.hxx"

namespace trash{

Restore::Restore(std::vector<std::string>& args, RestoreOptions& rOpt) : m_rOpt{rOpt}
{
#ifndef NDEBUG
	std::println("allOption is:      {}", m_rOpt.allOption);
	std::println("presentOption is:  {}", m_rOpt.pastOption);
	std::println("previousOption is: {}", m_rOpt.previousOption);
	std::println("verboseOption is:  {}", m_rOpt.verboseOption);
	std::println("forceOption is:    {}", m_rOpt.forceOption); //Unused
	std::println("silentOption is:   {}", m_rOpt.silentOption);
	std::println("sqlOption is:      {}", m_rOpt.sqlOption);
#endif
	Restore::file(args);
	if(m_rOpt.allOption)     Restore::allFile();
	if(m_rOpt.pastOption)    Restore::past();
	if(m_rOpt.previousOption)Restore::previous();
	if(m_rOpt.sqlOption)     Restore::sqlInjection();
}

void Restore::file(std::vector<std::string>& args)
{
	for(std::string& arg : args)
	{
		//TODO. Need to add something accounting for empty values returned from sql.
		std::string trashFile              { Database().selectData(std::format("SELECT file FROM trash WHERE id='{0}';", arg)) };
		std::filesystem::path originalPath { Database().selectData(std::format("SELECT path FROM trash WHERE id='{0}';", arg)) };

		if(checkTrashFile(trashFile) && checkOriginalPath(originalPath))
		{
			//Check for permissions on the original path
			if(!canMvFileChk(std::filesystem::directory_entry(originalPath)))
			{
				if(!m_rOpt.silentOption)
					std::println("process execution user {} is missing write/execute permissions for parent directory of {} ", singleton->getWorkingUsername(), originalPath.string());
				continue;
			}

			if(aci::Stat(singleton->getWorkingTrashDir().c_str()).st_dev() == aci::Stat(originalPath.parent_path().string().c_str()).st_dev())
			{
				//TODO. Place inside try catch and skip the current arg with continue:
				std::filesystem::rename(singleton->getWorkingTrashFileDir() + trashFile, originalPath);

				if(m_rOpt.verboseOption)
					std::println("Path is:{0}", originalPath.string().c_str());

				//TODO;
				//Also check against saveFileData() values to make sure.
				Database().executeSQL(std::format("DELETE FROM trash WHERE id='{0}';", arg));
			}
			else
			{
				//TODO. Place inside try catch and skip the current arg with continue:
				externRename((singleton->getWorkingTrashFileDir() + trashFile), originalPath);

				if(m_rOpt.verboseOption)
					std::println("Path is:{0}", originalPath.string());

				//TODO; //Also check against saveFileData() values to make sure.
				Database().executeSQL(std::format("DELETE FROM trash WHERE id='{0}';", arg));
				
			}

		}
		else
		{
			if(!m_rOpt.silentOption)
				std::println("restore failed: {0}", arg);
			//NOTICE: Add continue; if adding something below this line
		}
	}
}

void Restore::allFile()
{
	std::vector<std::string> vList { Database().selectDataB("SELECT id FROM trash;") };

#ifndef NDEBUG
	std::println("Printing all existing record IDs");
	for(auto str : vList)
	{
		std::println("{0}", str);
	}
#endif
	Restore::file(vList);
}

void Restore::past()
{
	for(auto format : m_rOpt.timeVec)
	{
		//TODO. add silence to guard the return cerr text
		long long timestamp{};
		int return_code = formatToTimestamp(format, timestamp);
		
		if (return_code == 0)
		{
			std::vector<std::string> vList { Database().selectDataB(std::format("SELECT id FROM trash WHERE timestamp > {};", timestamp)) };
			Restore::file(vList);
		}
		else if(return_code == 1 && !m_rOpt.silentOption) std::cerr << "error: units not found\n";
		else if(return_code == 2 && !m_rOpt.silentOption) std::cerr << "error: invalid format " << format << "\n";
		else if(return_code == 3 && !m_rOpt.silentOption) std::cerr << "unexpected failure\n";
	}
}

void Restore::previous()
{
	std::vector<std::string> vList { Database().selectDataB("SELECT id FROM trash WHERE execution=(SELECT MAX(execution) FROM trash);") };
	Restore::file(vList);
}

//Must return id. E.G "SELECT id FROM trash WHERE file='.hiddenfile';" 
void Restore::sqlInjection()
{
	for(auto sql : m_rOpt.sqlVec)
	{
#ifndef NDEBUG
		std::println("SQL Statement is: {}", sql);
#endif
		std::vector<std::string> vList { Database().selectDataB(sql) };
		Restore::file(vList);
	}
}

//Checks if trashFile Exists inside of /trash/file/<name>
bool Restore::checkTrashFile(const std::string& trashFile)
{
	std::filesystem::path filePath { singleton->getWorkingTrashFileDir() + trashFile };

	bool result = Verity(std::filesystem::directory_entry(filePath)).exists ? true : false;

	if(!result && !m_rOpt.silentOption)
		std::println("Failed to restore file: \"{0}\"\nfile to restore is missing in \"{1}\"", trashFile, singleton->getWorkingTrashFileDir());

	//TODO; Needs semantic correction
	return result;
}

bool Restore::checkOriginalPath(const std::filesystem::path& trashDir)
{
	bool result{};
	//check parent path exists
	if(Verity(std::filesystem::directory_entry(trashDir.parent_path())).exists)
	{
		if(Verity(std::filesystem::directory_entry(trashDir)).exists)
		{
			//TODO. Add an option to force move the file over upon user request through a prompt. overwriting the existing file
			if(m_rOpt.verboseOption)
			   std::println("failed on file check: \"{0}\"\nexisting file found inside DIR \"{1}\"", trashDir.filename().string(), trashDir.parent_path().string());

			result = false;
		}
		else
		{
			if(m_rOpt.verboseOption)
			   std::println("success on file check: \"{0}\"\nno existing file found inside DIR \"{1}\"", trashDir.filename().string(), trashDir.parent_path().string());

			result = true;
		}
	}
	else
	{
		//TODO. Add an option to recreate the directory if sufficient permissions are available.
		if(m_rOpt.verboseOption)
		   std::println("failed on file check: \"{0}\"\nparent directory not found: \"{1}\"", trashDir.filename().string(), trashDir.parent_path().string());
		result = false;
	}
	
	return result;
}

//Read relevant column database. Then perform checks
//Check if file trash name exists inside file/
//Check if the original trash directory exists/
//Check if the original trash directory contains the same filename.
//Create exception for directory permission as they could have changed.

}//namespace trash
