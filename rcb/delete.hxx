#pragma once
#ifndef DELETE_HXX
#define DELETE_HXX

#include <string>
#include <array>
#include <filesystem>
#include <vector>

#include "delete_args.hxx"

namespace rcb{

class Delete
{
public:
	Delete(std::vector<std::string>& args, DeleteOptions& dOpt);
private:
	DeleteOptions& m_dOpt;
	std::string m_currentExecutionID;
	
	void file(std::vector<std::string>& args);
	bool renameDupe(std::string& file);
	const std::array<std::string, 7> saveFileData(const std::string& trashFilename, const std::filesystem::path& originalDir);
	unsigned long long fileSize(const std::filesystem::directory_entry& file);
	std::string incrementExecutionID();

	bool has_trailing_slash(const std::filesystem::path& path);
	std::filesystem::path remove_trailing_slash(const std::filesystem::path& path);
};
	
}//namespace rcb

#endif
