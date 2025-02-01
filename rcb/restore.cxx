#include <filesystem>
#include <format>
#include <iostream>
#include <print>

#include "restore.hxx"
#include "platform/aci/aci.hxx"
#include "common/database.hxx"
#include "common/utils.hxx"
#include "common/globals.hxx"

namespace rcb{

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
		std::string stagedFile              { Database().selectData(std::format("SELECT file FROM {0} WHERE id='{1}';", g_progName, arg)) };
		std::filesystem::path originalPath { Database().selectData(std::format("SELECT path FROM {0} WHERE id='{1}';", g_progName, arg)) };

		if(checkProgFile(stagedFile) && checkOriginalPath(originalPath))
		{
			//Check for permissions on the original path
			if(!canMvFileChk(std::filesystem::directory_entry(originalPath)))
			{
				if(!m_rOpt.silentOption)
					std::println("process execution user {} is missing write/execute permissions for parent directory of {} ", g_singleton->getWorkingUsername(), originalPath.string());
				continue;
			}

			if(aci::Stat(g_singleton->getWorkingProgDir().string().c_str()).st_dev() == aci::Stat(originalPath.parent_path().string().c_str()).st_dev())
			{
				//TODO. Place inside try catch and skip the current arg with continue:
				std::filesystem::rename(g_singleton->getWorkingProgFileDir() / stagedFile, originalPath);

				if(m_rOpt.verboseOption)
					std::println("Path is:{0}", originalPath.string().c_str());

				//TODO;
				//Also check against saveFileData() values to make sure.
				Database().executeSQL(std::format("DELETE FROM {0} WHERE id='{1}';", g_progName, arg));
			}
			else
			{
				//TODO. Place inside try catch and skip the current arg with continue:
				externRename((g_singleton->getWorkingProgFileDir() / stagedFile), originalPath);

				if(m_rOpt.verboseOption)
					std::println("Path is:{0}", originalPath.string());

				//TODO; //Also check against saveFileData() values to make sure.
				Database().executeSQL(std::format("DELETE FROM {0} WHERE id='{1}';", g_progName, arg));
				
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
	//Need to restore files with the lowest depth ascending.
	std::vector<std::string> vList { Database().selectDataB(std::format("SELECT id FROM {0} ORDER BY depth ASC;", g_progName)) };

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
			std::vector<std::string> vList { Database().selectDataB(std::format("SELECT id FROM {0} WHERE timestamp > {1};", g_progName, timestamp)) };
			Restore::file(vList);
		}
		else if(return_code == 1 && !m_rOpt.silentOption) std::cerr << "error: units not found\n";
		else if(return_code == 2 && !m_rOpt.silentOption) std::cerr << "error: invalid format " << format << "\n";
		else if(return_code == 3 && !m_rOpt.silentOption) std::cerr << "unexpected failure\n";
	}
}

void Restore::previous()
{
	std::vector<std::string> vList { Database().selectDataB(std::format("SELECT id FROM {0} WHERE execution=(SELECT MAX(execution) FROM {0});", g_progName)) };
	Restore::file(vList);
}

//Must return id. E.G "SELECT id FROM rcb WHERE file='.hiddenfile';" 
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

//Checks if progFile Exists inside of /rcb/file/<name>
bool Restore::checkProgFile(const std::string& stagedFile)
{
	std::filesystem::path filePath { g_singleton->getWorkingProgFileDir() / stagedFile };

	bool result = Verity(std::filesystem::directory_entry(filePath)).exists ? true : false;

	if(!result && !m_rOpt.silentOption)
		std::println("Failed to restore file: \"{0}\"\nfile to restore is missing in \"{1}\"", stagedFile, g_singleton->getWorkingProgFileDir().string());

	//TODO; Needs semantic correction
	return result;
}

bool Restore::checkOriginalPath(const std::filesystem::path& progDir)
{
	bool result{};
	//check parent path exists
	if(Verity(std::filesystem::directory_entry(progDir.parent_path())).exists)
	{
		if(Verity(std::filesystem::directory_entry(progDir)).exists)
		{
			//TODO. Add an option to force move the file over upon user request through a prompt. overwriting the existing file
			if(m_rOpt.verboseOption)
			   std::println("failed on file check: \"{0}\"\nexisting file found inside DIR \"{1}\"", progDir.filename().string(), progDir.parent_path().string());

			result = false;
		}
		else
		{
			if(m_rOpt.verboseOption)
			   std::println("success on file check: \"{0}\"\nno existing file found inside DIR \"{1}\"", progDir.filename().string(), progDir.parent_path().string());

			result = true;
		}
	}
	else
	{
		//TODO. Add an option to recreate the directory if sufficient permissions are available.
		if(m_rOpt.verboseOption)
		   std::println("failed on file check: \"{0}\"\nparent directory not found: \"{1}\"", progDir.filename().string(), progDir.parent_path().string());
		result = false;
	}
	
	return result;
}

//Read relevant column database. Then perform checks
//Check if file name exists inside file/
//Check if the original rcb directory exists/
//Check if the original rcb directory contains the same filename.
//Create exception for directory permission as they could have changed.

}//namespace rcb
