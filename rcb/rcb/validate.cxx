#include <format>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <print>
#include <algorithm>

#include "validate.hxx"
#include "common/database.hxx"
#include "validate_args.hxx"
#include "common/globals.hxx"
#include "common/utils.hxx"

namespace rcb{

Validate::Validate(const ValidateOptions& vOpt) : m_vOpt{vOpt}  
{
#ifndef NDEBUG
	std::println("verboseOption is:     {}", m_vOpt.verboseOption);
	std::println("silentOption is:      {}", m_vOpt.silentOption);
	std::println("yesOption is:         {}", m_vOpt.yesOption);
	std::println("fileOption is:        {}", m_vOpt.fileOption);
	std::println("dataOption is:        {}", m_vOpt.dataOption);
	std::println("wipeOption is:        {}", m_vOpt.wipeOption);
	std::println("fillDirSizeOption is: {}", m_vOpt.fillDirSizeOption);
#endif
	if(m_vOpt.fileOption)Validate::file();
	if(m_vOpt.dataOption)Validate::data();
	if(m_vOpt.wipeOption)Validate::wipe();
	if(m_vOpt.fillDirSizeOption)Validate::fillDirectorySize();
}

void Validate::allFile()
{
	file();
	data();
	wipe();
}

void Validate::file()
{
	// check if the stagedFile is referenced inside data/ with any record.
	// Move to wipe/
	// Finally delete it.

	std::vector<std::string> danglingFiles{};

	for (const auto& entry : std::filesystem::directory_iterator(g_singleton->getWorkingProgFileDir()))
	{    
		std::string stagedFile = entry.path().filename().string();
		std::string dbProgFileRecord = m_db.selectData(std::format("SELECT file FROM {0} WHERE file='{1}';", g_progName, stagedFile));

		if(stagedFile != dbProgFileRecord)
			danglingFiles.push_back(stagedFile);
	}

	if(danglingFiles.size() == 0)
	{
		if(m_vOpt.verboseOption)
			std::println("no dangling file(s) found inside {}/", g_singleton->getWorkingProgFileDir().filename().string());
	}
	else if(danglingFiles.size() > 0)
	{
		bool confirmFlag{};
		
		if(m_vOpt.verboseOption)
		{
			std::println("dangling file(s) found in {}/:", g_singleton->getWorkingProgFileDir().filename().string());
			for(const auto& file : danglingFiles)
				std::println("{}", file);
		}
		if(!confirmFlag && !m_vOpt.yesOption && !m_vOpt.silentOption)
		{
			std::string confirm;
			std::println("permanently remove dangling files? [Y/n]"); 
			do std::getline(std::cin, confirm); 
			while(confirm != "Y" && confirm != "n");
			confirm == "Y" ? confirmFlag=true : confirmFlag=false;
		}
		if(confirmFlag || m_vOpt.yesOption || m_vOpt.silentOption)
		{
			for(const auto& file : danglingFiles)
			{
				//WARNING: If a file with the same name exists in wipe/ it will be overwritten. file/ only contains unique names but files in wipe/ are not checked.
				std::filesystem::rename(g_singleton->getWorkingProgFileDir() / file, g_singleton->getWorkingProgWipeDir() / file);          
				std::filesystem::remove_all(g_singleton->getWorkingProgWipeDir() / file);
			}
		}  
	}

}

void Validate::data()
{
	// Should check for:
	// records points to a stagedFile in /file
	// Then delete

	std::vector<std::string> danglingRecords { m_db.selectDataB(std::format("SELECT file from {0};", g_progName)) };

	std::vector<std::string> stagedFiles{};
	for (const auto& entry : std::filesystem::directory_iterator(g_singleton->getWorkingProgFileDir()))
		stagedFiles.push_back(entry.path().filename().string());

	//pop back dbProgFiles with whatever is inside program's file/.
	for (const auto& element : stagedFiles)
		danglingRecords.erase(std::remove(danglingRecords.begin(), danglingRecords.end(), element), danglingRecords.end());

	if(danglingRecords.size() == 0)
	{
		if(m_vOpt.verboseOption)
			std::println("no dangling record(s) found inside {}/", g_singleton->getWorkingProgDataDir().filename().string());
	}
	else if(danglingRecords.size() > 0)
	{
		bool confirmFlag{};
		
		if(m_vOpt.verboseOption)
		{
			std::println("Dangling record(s) found:");
			for (const auto& danglingRecord : danglingRecords)
				std::println("{}", danglingRecord);
		}
		if(!confirmFlag && !m_vOpt.yesOption && !m_vOpt.silentOption)
		{
			std::string confirm;
			std::println("permanently remove dangling record(s)? [Y/n]"); 
			do std::getline(std::cin, confirm); 
			while(confirm != "Y" && confirm != "n");
			confirm == "Y" ? confirmFlag=true : confirmFlag=false;
		}
		if(confirmFlag || m_vOpt.yesOption || m_vOpt.silentOption)
		{
			for (const auto& danglingRecord : danglingRecords)
				m_db.executeSQL(std::format("DELETE FROM {0} WHERE file='{1}';", g_progName, danglingRecord));
		}
	}

}

void Validate::wipe()
{
	//Check for existence of files first. Display them. Then remove...
	std::vector<std::filesystem::path> danglingFiles{};

	for (const auto& entry : std::filesystem::directory_iterator(g_singleton->getWorkingProgWipeDir()))
		danglingFiles.push_back(entry);

	if (danglingFiles.size() == 0)
	{
		if(m_vOpt.verboseOption)
			std::println("no dangling file(s) found inside {}/", g_singleton->getWorkingProgWipeDir().filename().string());
	}
	else if(danglingFiles.size() > 0)
	{   
		bool confirmFlag{};
		
		if(m_vOpt.verboseOption)
		{
			std::println("dangling file(s) found in {}/:", g_singleton->getWorkingProgWipeDir().filename().string());
			for(const auto& entry : danglingFiles)
				std::println("{}", entry.filename().string());
		}
		if(!confirmFlag && !m_vOpt.yesOption && !m_vOpt.silentOption)
		{
			std::string confirm;
			std::println("permanently remove dangling files? [Y/n]"); 
			do std::getline(std::cin, confirm); 
			while(confirm != "Y" && confirm != "n");
			confirm == "Y" ? confirmFlag=true : confirmFlag=false;
		}
		if(confirmFlag || m_vOpt.yesOption || m_vOpt.silentOption)
		{
			for (const auto& entry : std::filesystem::directory_iterator(g_singleton->getWorkingProgWipeDir()))
			{
				try
				{
					std::filesystem::remove_all(entry.path());
				}
				catch (std::runtime_error& e)
				{
					//REVISE: 
					/* NOTE: Exception likely occurs due to a permissions issue as remove_all recursively removes files, NOT directories, individually.
					It will only remove a directory when it's empty, otherwise it will continue to remove all contents inside.
					E.G If a directory has another directory that has root ownership with contents inside, they cannot be removed. 
					This is because the program is executed by a user that is missing the required permissions for the nested directory, but has permissions to remove contents in the top-level directory.

					The exception is then thrown. Executing the program as root allows the operation to finish without throwing any exceptions.
					*/
					if(!m_vOpt.silentOption) std::cerr << e.what() << std::endl;
					continue;
				}
			}
		} 
	}
} 

void Validate::fillDirectorySize()
{
	//Store NULL directories into vector
	//Optional: Check directory is in-fact a directory again. If not then remove from vector.
	//iterate directory and save all files excluding directories into a new vector list like in List::size().
	//Remove dupe ino,dev from new vector.
	//Stat all files and append them onto a long long
	//Update the directory size in the database using executeSQL().

	//REVISE: This should be vector of type std::filesystem::path
	std::vector<std::string> nullDirectoriesQuery { m_db.selectDataB(std::format("SELECT file from {0} WHERE filetype='directory' AND size='NULL';", g_progName)) };
	
	//prepend file/ path to each filename
	std::transform(nullDirectoriesQuery.begin(), 
				   nullDirectoriesQuery.end(), 
				   nullDirectoriesQuery.begin(), 
				   [](std::string& f) 
				   { return g_singleton->getWorkingProgFileDir().string() + f; });

	for(auto& directoryPathString : nullDirectoriesQuery)
	{
		//Check for read perms on directory, which is needed in order to iterate through it.
		if (!canReadDirChk(std::filesystem::directory_entry(directoryPathString)) &&
			Verity(std::filesystem::directory_entry(directoryPathString)).type == 
			std::filesystem::file_type::directory) 
		{
			if(!m_vOpt.silentOption)
				std::println("cannot save directory size, process execution user {} is missing read permissions for directory {}", 
				g_singleton->getWorkingUsername(), directoryPathString);
			continue;
		}

		auto directory_entry = std::filesystem::directory_entry(directoryPathString);
		unsigned long long size { directorySize(directory_entry) };
		m_db.executeSQL(std::format("UPDATE {0} SET size='{1}' WHERE file='{2}';", g_progName, size, directory_entry.path().filename().string()));
	}
}

}//namespace rcb
