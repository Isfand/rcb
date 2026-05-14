#include <algorithm>
#include <array>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <format>
#include <map>
#include <print>
#include <regex>
#include <set>
#include <string>

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
	switch (ft)
	{
		case std::filesystem::file_type::regular:   return "regular";
		case std::filesystem::file_type::directory: return "directory";
		case std::filesystem::file_type::symlink:   return "symlink";
		case std::filesystem::file_type::socket:    return "socket";
		case std::filesystem::file_type::fifo:      return "fifo";
		case std::filesystem::file_type::block:     return "block";
		case std::filesystem::file_type::character: return "character";
		case std::filesystem::file_type::unknown:   return "unknown";
		case std::filesystem::file_type::not_found: return "not_found";
		case std::filesystem::file_type::none:      return "none";
		default:                                    return "unrecognized";
	}
}

//Warning does not explicitly copy permissions and does not preserve ownership. Also doesn't check for free space.
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

//WARNING: does not recursively check read permissions.
//Note: access() can do the same thing
bool canReadDir(const std::filesystem::directory_entry& entry)
{
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

//Recursive read dir check
//TODO: Maybe make this function return which directory path does not have read access as a std::expected return?
bool canReadDirRec(const std::filesystem::directory_entry& dir)
{
	//Early guards to allow recursive_directory_iterator to do it's job
	//This is needed to make sure it's only directories that get passed through.
	if (Verity(dir).type != std::filesystem::file_type::directory)
		return false;
	
	//This is also needed to make sure the initial directory is readable or recursive_directory_iterator will fail.
	if (!canReadDir(dir)) return false;

	for (const auto& entry : std::filesystem::recursive_directory_iterator(dir.path())) 
	{
		std::error_code ec;
		std::filesystem::file_status status = std::filesystem::symlink_status(entry.path(), ec);

		if (ec) return false;

		//Note: For some reason is_directory fails on circular symlinks. Which is why this exists. Can't be used on the same if statement.
		if(!std::filesystem::is_symlink(status)) 
		{
			//Is dir for allowing files only of that type.
			if((std::filesystem::is_directory(entry.path())))
			{
				if (!canReadDir(entry)) return false;
			}
		}
	}

	return true;
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
		{"t"  , 1LL             },
		{"s"  , 1LL             },
		{"m"  , 60LL            },
		{"h"  , 3'600LL         },
		{"d"  , 86'400LL        },
		{"w"  , 604'800LL       },
		{"mo" , 2'629'746LL     },
		{"y"  , 31'556'952LL    },
		{"dec", 315'569'520LL   },
		{"c"  , 3'155'695'200LL },
		{"k"  , 31'556'952'000LL},
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

//Rename a file as (.)filename(n).*
//Multi-part extensions exist. They can also be generated. This is the safest option.
//Note: This does does ignore negative values and will just append (1).
void renameFile(std::string& file)
{
	std::string_view view(file);

	// Split by position — no hardcoded extensions needed
	std::size_t name_start = (!view.empty() && view[0] == '.') ? 1 : 0; // skip hidden dot
	auto ext_dot            = view.find('.', name_start);                // first real extension dot

	const std::string_view leading = view.substr(0, name_start);        // "" or "."
	std::string            base    = std::string(view.substr(name_start, ext_dot - name_start));
	const std::string_view ext     = (ext_dot == std::string_view::npos) ? "" : view.substr(ext_dot);

	// Increment (n) counter in base name
	if (base.size() >= 3 && base.back() == ')')
	{
		std::size_t open = base.rfind('(');
		if (open != std::string::npos && open < base.size() - 2)
		{
			std::string_view num_str(base.c_str() + open + 1, base.size() - open - 2);
			bool all_digits = !num_str.empty() && std::all_of(num_str.begin(), num_str.end(), ::isdigit);

			if (all_digits)
			{
				int n = std::stoi(std::string(num_str)) + 1;
				base  = base.substr(0, open + 1) + std::to_string(n) + ")";
				file  = std::string(leading) + base + std::string(ext);
				return;
			}
		}
	}

	// No (n) suffix yet — append (1)
	file = std::string(leading) + base + "(1)" + std::string(ext);
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
int sanitizeRemoveAll(const std::filesystem::path& path)
{
	std::error_code ec;

	//Note: Use Verity. ::exists ignores symlinks.
	//if (!std::filesystem::exists(path, ec))
		//return true;

	// Recursively iterate bottom-up
	for (auto it = std::filesystem::recursive_directory_iterator(
			path,
			std::filesystem::directory_options::skip_permission_denied,
			ec
		);
		it != std::filesystem::recursive_directory_iterator();
		++it)
	{
		//if (ec) return false;

		const std::filesystem::path& p = it->path();

		// Grant owner full permissions to allow deletion
		std::filesystem::permissions(
			p,
			std::filesystem::perms::owner_all,
			std::filesystem::perm_options::add,
			ec);

		//if (ec) return false;
	}

	// Make sure root directory is writable
	std::filesystem::permissions(
		path,
		std::filesystem::perms::owner_all,
		std::filesystem::perm_options::add,
		ec);

	//if (ec) return false;

	// Remove everything recursively
	std::filesystem::remove_all(path, ec);

	return !ec;
}

}//namespace rcb
