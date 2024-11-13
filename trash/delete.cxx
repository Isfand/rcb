#include <iostream>
#include <filesystem>
#include <regex>
#include <chrono>
#include <stdexcept>
#include <string>
#include <print>

#include "delete.hxx"
#include "delete_args.hxx"
#include "platform/aci/aci.hxx"
#include "common/database.hxx"
#include "common/globals.hxx"
#include "common/utils.hxx"

namespace trash{

Delete::Delete(std::vector<std::string>& args, DeleteOptions& dOpt) : m_dOpt{dOpt}
{
#ifndef NDEBUG
	std::println("verboseOption is:   {}", m_dOpt.verboseOption);
	std::println("forceOption is:     {}", m_dOpt.forceOption); //Unused
	std::println("silentOption is:    {}", m_dOpt.silentOption);
	std::println("noDirSizeOption is: {}", m_dOpt.noDirSizeOption);
#endif
	m_currentExecutionID = incrementExecutionID();
	Delete::file(args);
}

void Delete::file(std::vector<std::string>& args)
{
	for(std::string& file : args)
	{
		//UPDATE: weakly canonical can throw exception if a symlink chains to many symlinks. Replacing with custom solution.
		//CHANGED: systemFilePath should be in for loop top-level scope that way Verity can check symlink_status() properly. 
		const std::filesystem::path systemFilePath { remove_trailing_slash(std::filesystem::absolute(file).lexically_normal()) };

		if(Verity(std::filesystem::directory_entry(systemFilePath)).exists)
		{
			std::string mutFilename { systemFilePath.filename().string() };
			std::filesystem::directory_entry trashEntry { singleton->getWorkingTrashFileDir() + systemFilePath.filename().string() };

			//Need to use systemFilePath for the full path. Relativity creates issues.
			if (!canMvFileChk(std::filesystem::directory_entry(systemFilePath)))
			{
				if(!m_dOpt.silentOption)
					std::println("process execution user {} is missing write/execute permissions for parent directory of {} ", singleton->getWorkingUsername(), file);
				continue;
			}

			if(m_dOpt.verboseOption)
			{
				std::println("System filepath is: {0}", systemFilePath.string());
				std::println("Searching path: {}", singleton->getWorkingTrashFileDir() + systemFilePath.filename().string());
			}

			//To improve time. Reverse the logic of these two. if and else ifs. As the second is more common
			if(Verity trashEntryItem(trashEntry); trashEntryItem.exists)
			{
				// Print entry is a dupe
				if(m_dOpt.verboseOption)
					std::println("Existing entry found in {0} DIR: {1}", g_progName, singleton->getWorkingTrashFileDir());

				//Can make this into a else while instead of the else if below? Would have to swap the declaration and definitions inside
				do
				{
					if (!Delete::renameDupe(mutFilename))
					{
						if(!m_dOpt.silentOption)
							std::cerr << "delete failed. cannot rename file. unknown format: " << mutFilename << "\n";
						continue;
					}
					trashEntry.assign(singleton->getWorkingTrashFileDir() + mutFilename);
#ifndef NDEBUG
					std::println("Checking new name in DIR: {}", trashEntry.path().string());
#endif
				}
				while(Verity(trashEntry).exists); //Needs to recheck with new instance. Cannot use same instance. WARNING: can create infinite loop.
			}
			else if(!(trashEntryItem.exists))
			{
				//Print if unique
				if(m_dOpt.verboseOption)
					std::println("Creating new entry in {0} DIR: {1}", g_progName, singleton->getWorkingTrashFileDir());
			}
			else
			{
				//File deletion failed
				return;
			}

#ifndef NDEBUG
			std::println("CFP is: {0}\nMFN is: {1}", systemFilePath.string(), mutFilename);
#endif
			//The reason to always write the file data first is because the program could be terminated.
			//Its better to have dangling records than have some file moved which is not on record.
			Database db;
			try
			{
				//Checks for read perms on directory, which is needed in order to iterate through it to save its size.
				//Also checks for noDirSizeOption so the message doesn't print when using the option
				if (!m_dOpt.noDirSizeOption && 
					!canReadDirChk(std::filesystem::directory_entry(systemFilePath.string())) &&
					Verity(std::filesystem::directory_entry(systemFilePath.string())).type ==
					std::filesystem::file_type::directory) 
				{
					if(!m_dOpt.silentOption)
						std::println("cannot save directory size, process execution user {} is missing read permissions for directory {}",
						singleton->getWorkingUsername(), systemFilePath.string());
				}
				auto fileDetails = Delete::saveFileData(mutFilename, systemFilePath);
				db.insertData(fileDetails); // Could make InsertData Return bool for error checking instead of using try/catch
			}
			catch(const std::runtime_error& e)
			{
				std::println("{}", e.what());
				//Using continue to allow the other args to be processed.
				continue;
			}
//**********
			if(aci::Stat(systemFilePath.string().c_str()).st_dev() == aci::Stat(singleton->getWorkingTrashDir().c_str()).st_dev())
			{
				if(m_dOpt.verboseOption)
					std::println("local device detected");

				try
				{
					std::filesystem::rename(systemFilePath, singleton->getWorkingTrashFileDir() + mutFilename);
				}
				catch(const std::runtime_error& e)
				{
					//Insert data failed
					//FIX. If failed delete the data that was just recently entered. THIS IS NOT IDEAL. Create a more thread safe solution. *Added 2nd try catch above to prevent removing existing data if insertData fails.
					db.executeSQL(std::format("DELETE FROM trash WHERE id=(SELECT MAX(id) FROM trash);"));
					if(!m_dOpt.silentOption)
						std::println("cannot move file. insufficient permissions");
					continue;
				}
			}
			else
			{
				if(m_dOpt.verboseOption)
					std::println("external device detected");
				
				try
				{
					//Get device ID of getWorkingTrashDir and compare it to the file argument. If it's not the same. Then you are accessing an external device.
					externRename(systemFilePath, (singleton->getWorkingTrashFileDir() + mutFilename));
				}
				catch(const std::runtime_error& e)
				{
					//Insert data failed
					//FIX. If failed delete the data that was just recently entered. THIS IS NOT IDEAL. Create a more thread safe solution. *Added 2nd try catch above to prevent removing existing data if insertData fails.
					db.executeSQL(std::format("DELETE FROM trash WHERE id=(SELECT MAX(id) FROM trash);"));
					if(!m_dOpt.silentOption)
						std::println("cannot move file. insufficient permissions");
					continue;
				}
			}
//***********
		}
		else
		{
			if(!m_dOpt.silentOption)
				std::println("path doesn't exist: {0}", systemFilePath.string());
		}
	}
}

bool Delete::renameDupe(std::string& file)
{
	std::smatch m{};
	//std::string newFile{};

	if(std::regex patternA (R"(\((\d+)\)\.[^.]*?$|[^\.]\((\d+)\)$)"); 
	std::regex_search(file, m, patternA))
	{
#ifndef NDEBUG
		std::println("Matches *(1).ext");
		std::println("number: {}\nposition is: {}\nlength is: {}", (std::stoi(m[2]))+1, m.position(), m.length());
#endif
		file.replace(m.position(2), m.length(2), std::to_string((std::stoi(m[2]))+1));
		//newFile = file.replace(m.position(2), m.length(2), std::to_string((std::stoi(m[2]))+1));
	}// Matches *(1).ext
	else if(std::regex patternB (R"(^[^\.].+(\.).*[^\.]$)");
	std::regex_search(file, m, patternB))
	{
#ifndef NDEBUG
		std::println("Matches *.ext pos is {}, size is: {}", m.position(1), m.size());
#endif
		file.insert((m[1].first - file.begin()), "(1)");
		//newFile = file;
	}// Matches *.ext
	else if(std::regex patternC (R"((\.)$)");
	std::regex_search(file, m, patternC))
	{
#ifndef NDEBUG
		std::println("Matches * pos is {}, size is: {}", m.position(1), m.size());
#endif
		file.insert((m[1].first - file.begin()), "(1)");
		//newFile = file;
	}// Matches *
	else if(std::regex patternD (R"([^\.]($))");
	std::regex_search(file, m, patternD))
	{
#ifndef NDEBUG
		std::println("Matches *. pos is {}, size is: {}", m.position(1), m.size());
#endif
		file.insert((m[1].first - file.begin()), "(1)");
		//newFile = file;
	}// Matches *.
	else
	{
#ifndef NDEBUG
		std::println("No_match");
#endif
		return false;
		//newFile = "UNKNOWN_FILE_FORMAT";
	}

	return true;
}

const std::array<std::string, 7> Delete::saveFileData(const std::string& trashFilename, const std::filesystem::path& originalPath)
{
	auto currentTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	
	std::string fileName{trashFilename};
	std::string filePath{originalPath.string()};
	std::string timestamp{std::to_string(currentTime)};
	
	std::string fileByteSize = 
		(m_dOpt.noDirSizeOption && 
		Verity(std::filesystem::directory_entry(originalPath)).type == 
		std::filesystem::file_type::directory) || 
		(!canReadDirChk(std::filesystem::directory_entry(originalPath)) && 
		Verity(std::filesystem::directory_entry(originalPath)).type == 
		std::filesystem::file_type::directory) ? 
		"NULL" : std::to_string(Delete::fileSize(std::filesystem::directory_entry(originalPath)));

	std::string fileType{fileTypeToString(Verity(std::filesystem::directory_entry(originalPath)).type)};
	std::string workingUsername{singleton->getWorkingUsername()};
	std::string executionID{m_currentExecutionID};

	return std::array<std::string, 7>{fileName, filePath, timestamp, fileByteSize, fileType, workingUsername, executionID};
}

long long unsigned Delete::fileSize(const std::filesystem::directory_entry& file)
{
	unsigned long long size{}; //TODO: does not prevent integer overflow. cap to MAX.
	Verity entryItem{file};

	switch(entryItem.type)
	{
		case std::filesystem::file_type::regular:
			size = std::filesystem::file_size(file);
			break;

		case std::filesystem::file_type::symlink:
			size = std::string(std::filesystem::read_symlink(file).filename().string()).length();
			break;

		case std::filesystem::file_type::directory:
			size = directorySize(file);
			break;

		case std::filesystem::file_type::socket:
			size = 0;
			break;

		case std::filesystem::file_type::fifo:
			size = 0;
			break;

		case std::filesystem::file_type::block:
			size = 0;
			break;
		
		case std::filesystem::file_type::character:
			size = 0;
			break;

		case std::filesystem::file_type::unknown:
			size = 0;
			break;
		
		case std::filesystem::file_type::not_found:
			size = 0;
			break;

		case std::filesystem::file_type::none:
			size = 0;
			break;
			
		default:
			size = 0;
			break;
	}
	
	return size;
}

std::string Delete::incrementExecutionID()
{
	std::string highestExecution{};
	std::string selectedExecution = Database().selectData(std::format("SELECT execution FROM trash WHERE execution=(SELECT max(execution) FROM trash) AND id=(SELECT max(id) FROM trash);"));

	if (selectedExecution != "") highestExecution = std::to_string(std::stoll(selectedExecution) + 1);
	else if (selectedExecution == "") highestExecution = "1";

	return highestExecution;
}

bool Delete::has_trailing_slash(const std::filesystem::path& path)
{
	//TEMPORARY: Move to ACI
	//#if defined(__unix__) || defined(__APPLE__)
	//	char delimiter = '/';
	//#elif defined(_WIN32) && !defined(__CYGWIN__)
	//	char delimiter = '\\';
	//#endif

	std::string path_str = path.string();
	// Check if the last character is a forward slash or backslash for ntfs.
	return !path_str.empty() && (path_str.back() == std::filesystem::path::preferred_separator);
}

std::filesystem::path Delete::remove_trailing_slash(const std::filesystem::path& path)
{
	// If the path has a trailing slash, return the parent path
	if (has_trailing_slash(path)) 
		return path.parent_path();  // Return the parent path without trailing slashes

	return path;  // No trailing slash found, return the path as-is
}
		
}//namespace trash
