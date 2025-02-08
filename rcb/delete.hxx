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
	Delete(const std::vector<std::string>& args, const DeleteOptions& dOpt);
private:
	const DeleteOptions& m_dOpt;
	std::string m_currentExecutionID;
	
	void file(const std::vector<std::string>& args);
	const std::array<std::string, 8> saveFileData(const std::string& stageFilename, const std::filesystem::path& originalDir);
	unsigned long long fileSize(const std::filesystem::directory_entry& file);
	std::string incrementExecutionID();

	bool has_trailing_slash(const std::filesystem::path& path);
	std::filesystem::path remove_trailing_slash(const std::filesystem::path& path);
	int pathDepth(const std::filesystem::path& path);
};
	
}//namespace rcb

#endif
