#include <algorithm>
#include <filesystem>
#include <map>
#include <print>
#include <format>
#include <set>
#include <map>
#include <regex>
#include <array>
#include <chrono>
#include <ctime>
#include <string>
#include <unordered_set>

#include "utils.hxx"
#include "rcb/platform/aci/aci.hxx"
#include "globals.hxx"

namespace rcb{

Verity::Verity(const std::filesystem::directory_entry& entry)
{
	auto status = entry.symlink_status();
	this->type = status.type();
	this->exists = std::filesystem::exists(status);
}

std::string fileTypeToString(std::filesystem::file_type ft)
{
	std::map<std::filesystem::file_type, std::string> FileTypeMap =
	{ 
		{ std::filesystem::file_type::regular,   "regular" }, 
		{ std::filesystem::file_type::directory, "directory" }, 
		{ std::filesystem::file_type::symlink,   "symlink" }, 
		{ std::filesystem::file_type::socket,    "socket" }, 
		{ std::filesystem::file_type::fifo,      "fifo" }, 
		{ std::filesystem::file_type::block,     "block" }, 
		{ std::filesystem::file_type::character, "character" }, 
		{ std::filesystem::file_type::unknown,   "unknown" }, 
		{ std::filesystem::file_type::not_found, "not_found" }, 
		{ std::filesystem::file_type::none,      "none" }, 
	}; 

	//WARNING. Below can throw an exception if ft is not found inside map. TODO. Handling required like in Restore::present().
	return FileTypeMap.at(ft);
}

void externRename(const std::filesystem::path& filePath, const std::filesystem::path& newCopy)
{
	auto cpOpt = std::filesystem::copy_options::copy_symlinks |
				 std::filesystem::copy_options::recursive     |
				 std::filesystem::copy_options::overwrite_existing;

	if(canMvFileChk(std::filesystem::directory_entry(filePath)))
	{
		std::filesystem::copy(filePath, newCopy, cpOpt);
		sanitizeRemoveAll(filePath); //remove original.
	}
	else 
	{
		//TODO. Nothing in this file should print out anything. Change to return type bool for success or fail
		//std::println("missing write/execute permissions from parent directory");
	}
}

//Checks if you have move access to the file's directory
bool canMvFileChk(const std::filesystem::directory_entry& entry)
{	
	//Reference of ownership in nested array 0(r),1(w),2(x)
	//perms.at(0); //owner
	//perms.at(1); //group
	//perms.at(2); //other
	//Currently assumes entry has a parent path.
	//The parent directory of the file determins if you can move a file. Therefor we get perms of parent directory.
	auto perms = getFilePerms(entry.path().parent_path());
	aci::User user{};
	aci::Stat stat { entry.path().parent_path().string().c_str() };

	//Early check for root user which has unrestricted access to everything. The root groupID doesn't count.
	if (user.uid() == 0ULL)
		return true;

	//Check if any of user's groups match the file GID. 
	//User & Stat GID Match
	auto usgm = [&](aci::User& user, aci::Stat& stat) -> bool
	{
		for(auto& group : user.groups())
			if (group == stat.st_gid()) 
				return true; 

		return false;
	};

	if(user.uid() == stat.st_uid()) //Check if 'owner' perms allow for move access.
	{
		if(perms.at(0).at(1) && perms.at(0).at(2))
			return true;
	}
	else if(usgm(user, stat)) //Check if 'group' perms allow for move access
	{
		if(perms.at(1).at(1) && perms.at(1).at(2))
			return true;
	}
	else //Check if 'other' perms allow for move access
	{
		if(perms.at(2).at(1) && perms.at(2).at(2))
			return true;
	}
	return false;
}

bool canReadDirChk(const std::filesystem::directory_entry& entry)
{

	//TODO. throw exception if the entry is not a directory.

	auto perms = getFilePerms(entry.path());

	aci::User user{};
	aci::Stat stat { entry.path().string().c_str() };

	//Early check for root user which has unrestricted access to everything. The root groupID doesn't count.
	if (user.uid() == 0ULL)
		return true;

	//Check if any of user's groups match the file GID. 
	//User & Stat GID Match
	auto usgm = [&](aci::User& user, aci::Stat& stat) -> bool
	{
		for(auto& group : user.groups())
			if (group == stat.st_gid()) 
				return true; 

		return false;
	};

	if(user.uid() == stat.st_uid()) //Check if 'owner' perms allow for read access
	{
		if(perms.at(0).at(0))
			return true;
	}
	else if(usgm(user, stat)) //Check if 'group' perms allow for read access
	{
		if(perms.at(1).at(0))
			return true;
	}
	else //Check if 'other' perms allow for read access
	{
		if(perms.at(2).at(0))
			return true;
	}

	return false;
}

// Function to extract boolean permissions for owner, group, and others using std::array
std::array<std::array<bool, 3>, 3> getFilePerms(const std::filesystem::path& file)
{
	std::filesystem::perms p { std::filesystem::symlink_status(file).permissions() };

	//Keep K&R indent
	// Extract owner permissions
	std::array<bool, 3> ownerPerms = {
		(p& std::filesystem::perms::owner_read)   != std::filesystem::perms::none,
		(p& std::filesystem::perms::owner_write)  != std::filesystem::perms::none,
		(p& std::filesystem::perms::owner_exec)   != std::filesystem::perms::none
	};

	// Extract group permissions
	std::array<bool, 3> groupPerms = {
		(p& std::filesystem::perms::group_read)   != std::filesystem::perms::none,
		(p& std::filesystem::perms::group_write)  != std::filesystem::perms::none,
		(p& std::filesystem::perms::group_exec)   != std::filesystem::perms::none
	};

	// Extract others permissions
	std::array<bool, 3> othersPerms = {
		(p& std::filesystem::perms::others_read)  != std::filesystem::perms::none,
		(p& std::filesystem::perms::others_write) != std::filesystem::perms::none,
		(p& std::filesystem::perms::others_exec)  != std::filesystem::perms::none
	};

	return {ownerPerms, groupPerms, othersPerms};
}

//XXX. You cannot recursive_directory_iterator the directory if you cannot read it. Can fail on dirs with no read access.
unsigned long long directorySize(const std::filesystem::directory_entry& directory)
{
	unsigned long long size{};
	std::vector<std::tuple<std::filesystem::directory_entry, unsigned long long, unsigned long long>> directoryEntries;
	std::vector<std::tuple<std::filesystem::directory_entry, unsigned long long, unsigned long long>> dedupedDirEntries;
	std::set<std::pair<unsigned long long, unsigned long long>> seen;

	for (const auto& entry : std::filesystem::recursive_directory_iterator(directory.path())) 
	{
		std::filesystem::file_status status = std::filesystem::symlink_status(entry.path());
		//Note: For some reason is_directory fails on circular symlinks. Which is why this exists. Can't be used on the same if statement.
		if(!std::filesystem::is_symlink(status)) 
		{
			//Is not dir is for preventing dir duplication.
			if(!std::filesystem::is_directory(entry.path()))
			{
				aci::Stat stat = entry.path().string().c_str();
				directoryEntries.push_back(std::make_tuple(entry, stat.st_ino(), stat.st_dev()));
			}
		}
		else
		{
			//if(!std::filesystem::is_directory(status)) continue;
			aci::Stat stat = entry.path().string().c_str();
			directoryEntries.push_back(std::make_tuple(entry, stat.st_ino(), stat.st_dev()));
		}
	}
	for (const auto& item : directoryEntries) 
	{
		unsigned long long second { std::get<1>(item) };
		unsigned long long third  { std::get<2>(item) };
		
		// Check if the pair (second, third) is already in the set
		if (seen.emplace(second, third).second) 
		{
			// If not, add the tuple to the deduped vector
			dedupedDirEntries.push_back(item);
		}
	}
	#ifndef NDEBUG
			// Print the deduplicated tuples
			for (const auto& item : dedupedDirEntries) 
				std::println("{}, {}, {}", std::get<0>(item).path().string(), std::get<1>(item), std::get<2>(item));
	#endif
	for (const auto& item : dedupedDirEntries) 
	{
		Verity entryItem { std::get<0>(item) };

		switch(entryItem.type)
		{
			case std::filesystem::file_type::regular:
				size += std::filesystem::file_size(std::get<0>(item));
				break;

			case std::filesystem::file_type::symlink:
				size += std::string(std::filesystem::read_symlink(std::get<0>(item)).filename().string()).length();
				break;

			case std::filesystem::file_type::socket:
				size += 0;
				break;

			case std::filesystem::file_type::fifo:
				size += 0;
				break;

			case std::filesystem::file_type::block:
				size += 0;
				break;

			case std::filesystem::file_type::character:
				size += 0;
				break;

			case std::filesystem::file_type::unknown:
				size += 0;
				break;

			case std::filesystem::file_type::not_found:
				size += 0;
				break;

			case std::filesystem::file_type::none:
				size += 0;
				break;

			default:
				size += 0;
				break;
		}
	}
	return size;
}

int formatToTimestamp(const std::string& format, long long& timestamp)
{
	//TODO. allow negative posix timestamps using '-' for before 1970. Need to account for time travelers. Can utilize '--'
	auto currentTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	//May run into Y:292,277,026,596 problem. I.E integer overflow.
	//Using year as: 365.2425
	std::map<std::string, long long> timeMap = 
	{ 
		{"t"  , 1LL                     },
		{"s"  , 1LL                     },
		{"m"  , 60LL                    },
		{"h"  , 3'600LL                 },
		{"d"  , 86'400LL                },
		{"w"  , 604'800LL               },
		{"mo" , 2'592'000LL             },
		{"y"  , 31'556'952LL            },
		{"dec", 315'619'200LL           },
		{"c"  , 3'155'692'600LL         },
		{"k"  , 31'556'926'000LL        },
		{"ae" , 31'556'926'000'000'000LL},
	};

	std::regex pattern(R"(^(\d+)([^\d].*)$)");
	std::smatch m;

	if (std::regex_search(format, m, pattern)) 
	{
		std::string digits { m[1].str() }; //^(\d+)
		std::string units  { m[2].str() }; //([^\d].*)$

#ifndef NDEBUG
		std::println("digits:        {}", digits);
		std::println("letters:       {}", units);
#endif
		if (auto it = timeMap.find(units); it != timeMap.end()) 
		{
			timestamp = (units == "t") ? std::stoll(digits) : currentTime - (std::stoll(digits) * timeMap.at(units));
#ifndef NDEBUG
			std::println("Found:         {}", it->second);
			std::println("total seconds: {}", std::stoll(digits) * timeMap.at(units));
			std::println("timestamp:     {}", timestamp);
#endif
			return 0;
		} 
		else 
		{
			//std::cerr << "error: unit " << units << " not found\n";
			return 1;
		}
	} 
	else 
	{
		//std::cerr << "error: invalid format " << format;
		return 2;
	}

	return 3;
}

//Unused
std::string posixTimeToDateTime(std::chrono::seconds timestamp)
{
	std::chrono::system_clock::time_point tp(timestamp);

	std::time_t time = std::chrono::system_clock::to_time_t(tp);

	std::tm *tm_info = std::localtime(&time);

	//ISO 8601 format
	return std::format("{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}",
		tm_info->tm_year + 1900,   // tm_year is years since 1900
		tm_info->tm_mon + 1,       // tm_mon is months since January (0-11)
		tm_info->tm_mday,          // Day of the month (1-31)
		tm_info->tm_hour,          // Hour (0-23)
		tm_info->tm_min,           // Minute (0-59)
		tm_info->tm_sec);          // Second (0-59)
}

//Unimplemented
std::string dataUnitConversion()
{
	//Data Unit Scaling
	return "";
}

//DESCRIPTION: renameFile renames the filename(string) by first getting the stem()
//Getting the extension by checking stringDifference() between the stem() and the originalFile's filename.
//Incrementing the filename in format *(n)
//Finally combining the the incremented filename with the extension.
void renameFile(std::string& file)
{
	const std::string originalFile = file;
	std::string mutFile = file;

	// Set of known multi-part extensions
	// Can make these into regexs instead of hard-coded values. E.G .tar.*, .7z.* or .tar.xz.* for any extension.
	const std::unordered_set<std::string> knownMultiPartExtensions =
	{
		".tar.gz", ".tar.bz2", ".tar.xz", ".tar.zst", ".tar.lz", ".tar.lzma"
	};

	// Function to remove known multi-part extensions & single extensions
	auto getStem = [&knownMultiPartExtensions](const std::filesystem::path& filePath) -> std::string
	{
		std::filesystem::path temp = filePath;
		std::string ext = temp.extension().string(); //unused
	
		// Check if the full known extension exists
		for (const std::string& knownExt : knownMultiPartExtensions)
		{
			if (temp.string().size() > knownExt.size() && 
				temp.string().substr(temp.string().size() - knownExt.size()) == knownExt)
			{
				// Remove full known extension
				return temp.string().substr(0, temp.string().size() - knownExt.size());
			}
		}

		// If no known multi-part extension, just return regular stem()
		return temp.stem().string();
	};

	//Used for getting extension.
	auto stringDifference = [](const std::string& str1, const std::string& str2) -> std::string
	{
		auto [it1, it2] = std::mismatch(str1.begin(), str1.end(), str2.begin(), str2.end());
		return (str1.size() > str2.size()) ? std::string(it1, str1.end()) : std::string(it2, str2.end());
	};
	
	auto incrementFilename = [](std::string& filename) -> std::string
	{
		// Check if the filename is long enough to have "(n)" at the end
		if (filename.size() < 4)
		{
			return filename + "(1)";  // Too short, so just append (1)
		}
	
		// Check if the filename ends with ')'
		if (filename[filename.size() - 1] != ')')
		{
			return filename + "(1)";  // No parentheses at the end, so append (1)
		}
	
		// Find the position of the last '(' before ')'
		size_t openParenPos = filename.rfind('(');
		if (openParenPos == std::string::npos || openParenPos == filename.size() - 2)
		{
			return filename + "(1)";  // No '(' or no digits between parentheses
		}
	
		// Check if the characters between '(' and ')' are digits
		bool isValidNumber = true;
		for (size_t i = openParenPos + 1; i < filename.size() - 1; ++i)
		{
			if (!std::isdigit(filename[i])) {
				isValidNumber = false;
				break;
			}
		}
	
		if (isValidNumber)
		{
			// Extract the number between parentheses and increment it
			size_t startPos = openParenPos + 1;
			size_t endPos = filename.size() - 1;
			int number = std::stoll(filename.substr(startPos, endPos - startPos)) + 1;
			
			// Create new filename with incremented number
			filename = filename.substr(0, openParenPos + 1) + std::to_string(number) + ")";
		}
		else
		{
			// If the parentheses don't contain a valid number, append (1)
			filename = filename + "(1)";
		}
	
		// Return the modified filename with the original extension
		return filename;
	};

	// Get stem
	mutFile = getStem(mutFile);

	// Extract the extension using stringDifference
	std::string extension = stringDifference(mutFile, originalFile);

	// Increment the filename and append the correct extension
	file = incrementFilename(mutFile) + extension;
}

//TODO. The first two can be passed as a single variable. Needs Revising.
//directory is where the checks will be made for recursive searching. This is immutable.
//stagePath is the entire filepath for where the file actually is. This is internal

//mutFilename is the filename that will be renamed if a dupe is found. Passed as mutable reference.
bool renameDupe(
	const std::filesystem::path& directory, 
	std::filesystem::directory_entry stagePath,
	std::string& mutFilename)
{
//To improve time. Reverse the logic of these two. if and else ifs. As the second is more common
	if(Verity stageEntryItem(stagePath); stageEntryItem.exists)
	{
		// Print entry is a dupe
		//if(Opt.verboseOption)
#ifndef NDEBUG
		std::println("Existing entry found in {0} DIR: {1}", g_kProgName, g_singleton->getWorkingProgFileDir().string());
#endif

		//Can make this into a else while instead of the else if below? Would have to swap the declaration and definitions inside
		do
		{
			renameFile(mutFilename);
			stagePath.assign(directory / mutFilename);
#ifndef NDEBUG
			std::println("Checking new name in DIR: {}", stagePath.path().string());
#endif
		}
		while(Verity(stagePath).exists); //Needs to recheck with new instance. Cannot use same instance. WARNING: can create infinite loop.

		return true;
	}
	else if(!(stageEntryItem.exists))
	{
		//Print if unique
		//if(Opt.verboseOption)
#ifndef NDEBUG
		std::println("Check for target path passed {0} DIR: {1}", g_kProgName, g_singleton->getWorkingProgFileDir().string());
#endif
		return true;
	}
	else 
	{
		//Path check failed
		return false;
	}

	return false;
}

std::filesystem::path deepestExistingPath(const std::filesystem::path& fullPath) 
{
	std::filesystem::path currentPath;
	for (const auto& part : fullPath)
	{
		currentPath /= part;
		if (!std::filesystem::exists(currentPath))
		{
			return currentPath.parent_path();  // Return the last existing directory
		}
	}
	return fullPath;  // Entire path exists
}

//Changes permissions before deleting. functionally equivent to rm -rf.
bool sanitizeRemoveAll(const std::filesystem::path& path)
{
	std::error_code ec;

	if (!std::filesystem::exists(path, ec))
		return true;

	// Recursively iterate bottom-up
	for (auto it = std::filesystem::recursive_directory_iterator(
			 path,
			 std::filesystem::directory_options::skip_permission_denied,
			 ec);
		 it != std::filesystem::recursive_directory_iterator();
		 ++it)
	{
		if (ec) return false;

		const std::filesystem::path& p = it->path();

		// Grant owner full permissions to allow deletion
		std::filesystem::permissions(
			p,
			std::filesystem::perms::owner_all,
			std::filesystem::perm_options::add,
			ec);

		if (ec) return false;
	}

	// Make sure root directory is writable
	std::filesystem::permissions(
		path,
		std::filesystem::perms::owner_all,
		std::filesystem::perm_options::add,
		ec);

	if (ec) return false;

	// Remove everything recursively
	std::filesystem::remove_all(path, ec);

	return !ec;
}



}//namespace rcb
