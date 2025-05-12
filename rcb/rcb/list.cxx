#include <format>
#include <iostream>
#include <print>
#include <filesystem>
#include <tuple>
#include <set>
#include <utility>

#include "list.hxx"
#include "list_args.hxx"
#include "platform/aci/aci.hxx"
#include "common/database.hxx"
#include "common/globals.hxx"
#include "common/utils.hxx"

namespace rcb{

List::List(const ListOptions& lOpt) : m_lOpt{lOpt}
{
#ifndef NDEBUG
	std::println("Inside List:");
	std::println("defaultOption is:       {}", m_lOpt.defaultOption);
	std::println("allOption is:           {}", m_lOpt.allOption);
	std::println("totalSizeOption is:     {}", m_lOpt.totalSizeOption);
	std::println("totalCountOption is:    {}", m_lOpt.totalCountOption);
	std::println("humanReadableOption is: {}", m_lOpt.humanReadableOption);
	std::println("verboseOption is:       {}", m_lOpt.verboseOption); //Unused
	std::println("noFormatOption is:      {}", m_lOpt.noFormatOption);
	std::println("pastOption is:          {}", m_lOpt.pastOption);
	std::println("previousOption is:      {}", m_lOpt.previousOption);
	std::println("sqlOption is:           {}", m_lOpt.sqlOption);
#endif

	//Order matters

	//TEMPORARY:
	//WARNING:
	//REVISE: This is hard-coded. Create something that dynamically gets the column names.
	//WIll also need a way to convert size of bytes to other units.
	//Can pair this problem with --no-format to work with regular queries.
	if(m_lOpt.humanReadableOption) { m_defaultSQLQuery = std::format("SELECT id, file, path, datetime(timestamp, 'unixepoch') AS timestamp, size, filetype, user, execution FROM {0}", g_kProgName); }

	if(m_lOpt.defaultOption)   List::allFile();
	if(m_lOpt.totalSizeOption) List::size();
	if(m_lOpt.totalCountOption)List::count();
	if(m_lOpt.pastOption)      List::past();
	if(m_lOpt.previousOption)  List::previous();
	if(m_lOpt.sqlOption)       List::sqlInjection();

	if(!(m_lOpt.totalSizeOption || 
		m_lOpt.totalCountOption || 
		m_lOpt.pastOption       || 
		m_lOpt.previousOption   || 
		m_lOpt.sqlOption))
		List::allFile();
}

//Prints all contents of rcb/data/*.sqlite3 database 
void List::allFile()
{
	std::print("Results:\n{}", m_db.selectDataA(m_defaultSQLQuery + ";"));
}

void List::file(const std::vector<std::string>& args)
{
	std::println("Results:");

	for(auto& arg : args)
		std::print("{}", m_db.selectDataA(std::format("{} WHERE id='{}';", m_defaultSQLQuery, arg)));
}

void List::past()
{
	//TODO. add silence to guard the return cerr text
	for(auto format : m_lOpt.timeVec)
	{
		long long timestamp{};
		int return_code = formatToTimestamp(format, timestamp);

		if (return_code == 0)
		{
			std::vector<std::string> vList = m_db.selectDataB(std::format("SELECT id FROM {0} WHERE timestamp > {1};", g_kProgName, timestamp));
			List::file(vList);
		}
		else if(return_code == 1) std::cerr << "error: units not found\n";
		else if(return_code == 2) std::cerr << "error: invalid format " << format << "\n";
		else if(return_code == 3) std::cerr << "unexpected failure\n";
	}
}

void List::previous()
{
	std::vector<std::string> vList { m_db.selectDataB(std::format("SELECT id FROM {0} WHERE execution=(SELECT MAX(execution) FROM {0});", g_kProgName)) };
	List::file(vList);
}

void List::sqlInjection()
{
	std::println("Results:");
	for(auto i { 0UL }; i < m_lOpt.sqlVec.size(); ++i)
	{
		std::print("{}", m_db.selectDataA(m_lOpt.sqlVec.at(i)));
		if(!(i + 1 >= m_lOpt.sqlVec.size()))
			std::print("---\n");
	}
}

void List::count()
{
	std::string descriptor{"total count: "};
	if(m_lOpt.noFormatOption)
		descriptor.erase();

	//Repeated in size();
	if(m_validFiles.empty())
		for (const auto& entry : std::filesystem::directory_iterator(g_singleton->getWorkingProgFileDir()))
		{
			std::string stagedFile = entry.path().filename().string();
			std::string dbProgFileRecord = m_db.selectData(std::format("SELECT file FROM {0} WHERE file='{1}';", g_kProgName, stagedFile));

			if(stagedFile == dbProgFileRecord)
				m_validFiles.push_back(stagedFile);
		};
	std::println("{}{}", descriptor, m_validFiles.size());
}

void List::size()
{
	std::string descriptor {"total size: "};
	if(m_lOpt.noFormatOption)
		descriptor.erase();

	std::vector<std::tuple<std::string, unsigned long long, unsigned long long>> entries;
	std::vector<std::tuple<std::string, unsigned long long, unsigned long long>> deduped;
	std::set<std::pair<unsigned long long, unsigned long long>> seen;
	unsigned long long size{};

	//Repeated in count();
	//Makes sure the filename exists inside file/
	if(m_validFiles.empty())
		for (const auto& entry : std::filesystem::directory_iterator(g_singleton->getWorkingProgFileDir()))
		{
			std::string stagedFile { entry.path().filename().string() };
			std::string dbProgFileRecord { m_db.selectData(std::format("SELECT file FROM {0} WHERE file='{1}';", g_kProgName, stagedFile)) };

			if(stagedFile == dbProgFileRecord)
				m_validFiles.push_back(stagedFile);
		};

	for(auto& validFile : m_validFiles)
	{
		aci::Stat stat { (g_singleton->getWorkingProgFileDir() / validFile).string().c_str() };
		entries.push_back(std::make_tuple(validFile, stat.st_ino(), stat.st_dev()));
	}

	for (const auto& item : entries) 
	{
		unsigned long long second = std::get<1>(item);
		unsigned long long third = std::get<2>(item);
		
		// Check if the pair (second, third) is already in the set
		if (seen.emplace(second, third).second) 
		{
			// If not, add the tuple to the deduped vector
			deduped.push_back(item);
		}
	}

#ifndef NDEBUG
		// Print the deduplicated tuples
		for (const auto& item : deduped) 
			std::println("{}, {}, {}", std::get<0>(item), std::get<1>(item), std::get<2>(item));
#endif

	for (const auto& de : deduped)
	{
		std::string query { m_db.selectData(std::format("SELECT size FROM {0} WHERE file='{1}';", g_kProgName, std::get<0>(de))) };
		std::string byte = (query == "NULL") ? "0" : query;
		size += std::stoull(byte);
		//WARNING. Be wary of NULL values in size.
		//Could also continue instead of assigning NULL to 0.
		//Can also use COALESCE or IFNULL in the query itself.
	}
	std::println("{}{}", descriptor, size);

}

}//namespace rcb