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

Restore::Restore(const std::vector<std::string>& args, const RestoreOptions& rOpt) : m_rOpt{rOpt}
{
#ifndef NDEBUG
	std::println("allOption is:              {}", m_rOpt.allOption);
	std::println("presentOption is:          {}", m_rOpt.pastOption);
	std::println("previousOption is:         {}", m_rOpt.previousOption);
	std::println("verboseOption is:          {}", m_rOpt.verboseOption);
	std::println("forceOption is:            {}", m_rOpt.forceOption); 
	std::println("forceReplaceOption is:     {}", m_rOpt.forceReplaceOption);
	std::println("forceRenameOption is:      {}", m_rOpt.forceRenameOption);
	std::println("forceRecreateDirectory is: {}", m_rOpt.forceRecreateDirectoryOption);
	std::println("silentOption is:           {}", m_rOpt.silentOption);
	std::println("sqlOption is:              {}", m_rOpt.sqlOption);
#endif
	Restore::file(args);
	if(m_rOpt.allOption)     Restore::allFile();
	if(m_rOpt.pastOption)    Restore::past();
	if(m_rOpt.previousOption)Restore::previous();
	if(m_rOpt.sqlOption)     Restore::sqlInjection();
}

//TODO: Reading the order is convoluted. Refactor where possible.
void Restore::file(const std::vector<std::string>& args)
{
	for(const std::string& arg : args)
	{
		//TODO. Need to add something accounting for empty values returned from sql.
		const std::string stagedFile             { m_db.selectData(std::format("SELECT {0} FROM {1} WHERE id='{2}';", g_kSchemaFile, g_kProgName, arg)) };
		const std::filesystem::path originalPath { m_db.selectData(std::format("SELECT {0} FROM {1} WHERE id='{2}';", g_kSchemaPath, g_kProgName, arg)) };

		//Early return for if no file exists inside of .rcb/file. Saves time.
		if(!progFileExists(stagedFile)) continue;

		const std::filesystem::file_type originalPathType = Verity(std::filesystem::directory_entry(originalPath)).type;

		std::string mutFilename              { originalPath.filename().string() };
		std::filesystem::path mutRestorePath { originalPath.parent_path() / mutFilename };

		//NOTE: forceReplaceOption is only used to enter the block. It does not change control flow like the others because rename() by default replaces regular files.
		if(PathStatus pathStatus = (originalPathStatus(originalPath)); 
		   (pathStatus == PathStatus::Free) 
		|| (pathStatus == PathStatus::Occupied && m_rOpt.forceReplaceOption)           // order 2
		|| (pathStatus == PathStatus::Occupied && m_rOpt.forceRenameOption)            // order 1
		|| (pathStatus == PathStatus::NoParent && m_rOpt.forceRecreateDirectoryOption) // order 0
		)
		{
			if(m_rOpt.forceRecreateDirectoryOption)
			{
				if(std::filesystem::path dep = deepestExistingPath(originalPath); 
					canMvFileChk(std::filesystem::directory_entry(dep)))
				{
					//Try catch is here as a just in case. It should never be triggered.
					try
					{
						if (std::filesystem::create_directories(originalPath.parent_path()))
						{
							if(m_rOpt.verboseOption)
								std::println("directories created successfully: {0}", originalPath.string());
						}
						else 
						{
							if(m_rOpt.verboseOption)
								std::println("directories already exist or creation failed: {}", originalPath.string());
						};
					}
					catch(const std::exception& e)
					{
						if(!m_rOpt.silentOption) std::cerr << "Error creating directories: " << e.what() << std::endl;
					}
				}
				else
				{
					if(!m_rOpt.silentOption)
						std::println("no (w,x) permissions for {0}. cannot force recreate original directory path", dep.string());
					continue;
				};
			}

			if (m_rOpt.forceRenameOption)
			{
				//NOTE: .parent_path() when applied to root it will return root itself /.
				if(!renameDupe(originalPath.parent_path(), std::filesystem::directory_entry(originalPath), mutFilename)) continue;

				//reassign
				mutRestorePath.assign(originalPath.parent_path() / mutFilename);
			}

			//Check for permissions on the original path
			if(!canMvFileChk(std::filesystem::directory_entry(originalPath)))
			{
				if(!m_rOpt.silentOption)
					std::println("process execution user {} is missing write/execute permissions for parent directory of {} ", g_singleton->getWorkingUsername(), originalPath.string());
				continue;
			}

			//NOTE: rename() cannot rename directories that are not empty. for that sanitizeRemoveAll() needs to be used before it.
			//Check if the path is internal or external
			if(aci::Stat(g_singleton->getWorkingProgDir().string().c_str()).st_dev() == aci::Stat(originalPath.parent_path().string().c_str()).st_dev())
			{
				try
				{
					//Duplicated #1
					if(m_rOpt.forceReplaceOption && originalPathType == std::filesystem::file_type::directory && pathStatus == PathStatus::Occupied) 
						sanitizeRemoveAll(originalPath);

					std::filesystem::rename(g_singleton->getWorkingProgFileDir() / stagedFile, mutRestorePath);
				}
				catch(const std::exception& e)
				{
					if(!m_rOpt.silentOption) std::cerr << e.what() << std::endl;
					continue;
				}

				if(m_rOpt.verboseOption)
					std::println("Path is:{0}", mutRestorePath.string().c_str());

				//TODO;
				//Also check against saveFileData() values to make sure.
				m_db.executeSQL(std::format("DELETE FROM {0} WHERE {1}='{2}';", g_kProgName, g_kSchemaID, arg));
			}
			else
			{
				try
				{
					//Duplicated #1
					if(m_rOpt.forceReplaceOption && originalPathType == std::filesystem::file_type::directory && pathStatus == PathStatus::Occupied) 
						sanitizeRemoveAll(originalPath);

					externRename((g_singleton->getWorkingProgFileDir() / stagedFile), mutRestorePath);
				}
				catch(const std::exception& e)
				{
					if(!m_rOpt.silentOption) std::cerr << e.what() << std::endl;
					continue;
				}

				if(m_rOpt.verboseOption)
					std::println("Path is:{0}", mutRestorePath.string());

				//TODO; //Also check against saveFileData() values to make sure.
				m_db.executeSQL(std::format("DELETE FROM {0} WHERE {1}='{2}';", g_kProgName, g_kSchemaID, arg));
				
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
	std::vector<std::string> vList { m_db.selectDataB(std::format("SELECT {0} FROM {1} ORDER BY {2} ASC;", g_kSchemaID, g_kProgName, g_kSchemaPathDepth)) };

#ifndef NDEBUG
	std::println("Printing all existing record IDs");
	for(auto str : vList)
	{
		std::println("{0}", str);
	}
#endif
	Restore::file(vList);
}

//TODO: past() is very similar across erase, list, restore. Find a way to share past()
void Restore::past()
{
	for(auto format : m_rOpt.timeVec)
	{
		long long timestamp{};
		int return_code = formatToTimestamp(format, timestamp);
		
		if (return_code == 0)
		{
			std::vector<std::string> vList { m_db.selectDataB(std::format("SELECT {0} FROM {1} WHERE {2} >= {3};", g_kSchemaID, g_kProgName, g_kSchemaTimestamp, timestamp)) };
			Restore::file(vList);
		}
		else if(return_code == 1 && !m_rOpt.silentOption) std::cerr << "error: units not found\n";
		else if(return_code == 2 && !m_rOpt.silentOption) std::cerr << "error: invalid format " << format << "\n";
		else if(return_code == 3 && !m_rOpt.silentOption) std::cerr << "unexpected failure\n";
	}
}

void Restore::previous()
{
	std::vector<std::string> vList { m_db.selectDataB(std::format("SELECT {0} FROM {1} WHERE {2}=(SELECT MAX({2}) FROM {1});", g_kSchemaID, g_kProgName, g_kSchemaExecution)) };
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
		std::vector<std::string> vList { m_db.selectDataB(sql) };
		Restore::file(vList);
	}
}

//Checks if progFile Exists inside of /rcb/file/<name>
bool Restore::progFileExists(const std::string& stagedFile)
{
	//Early return check to see if file is empty. This should also be handled from the sql querying function.
	if(stagedFile.empty()) return false;
	
	std::filesystem::path filePath { g_singleton->getWorkingProgFileDir() / stagedFile };

	bool result = Verity(std::filesystem::directory_entry(filePath)).exists; // ? true : false;

	if(!result && !m_rOpt.silentOption)
		std::println("Failed to restore file: \"{0}\"\nfile to restore is missing in \"{1}\"", stagedFile, g_singleton->getWorkingProgFileDir().string());

	//TODO; Needs semantic correction
	return result;
}

//return original path status
Restore::PathStatus Restore::originalPathStatus(const std::filesystem::path& progDir)
{
	PathStatus result{};
	//check parent path exists
	if(Verity(std::filesystem::directory_entry(progDir.parent_path())).exists)
	{
		if(Verity(std::filesystem::directory_entry(progDir)).exists)
		{
			if(m_rOpt.verboseOption)
				std::println("Occupied: \"{0}\"\nexisting file found inside DIR \"{1}\"", progDir.filename().string(), progDir.parent_path().string());
			result = PathStatus::Occupied;
		}
		else
		{
			if(m_rOpt.verboseOption)
				std::println("Free: \"{0}\"\nno existing file found inside DIR \"{1}\"", progDir.filename().string(), progDir.parent_path().string());
			result = PathStatus::Free;
		}
	}
	else
	{
		if(m_rOpt.verboseOption)
		   std::println("NoParent: \"{0}\"\nparent directory not found: \"{1}\"", progDir.filename().string(), progDir.parent_path().string());
		result = PathStatus::NoParent;
	}
	
	return result;
}

//Read relevant column database. Then perform checks
//Check if file name exists inside file/
//Check if the original rcb directory exists/
//Check if the original rcb directory contains the same filename.
//Create exception for directory permission as they could have changed.

}//namespace rcb
