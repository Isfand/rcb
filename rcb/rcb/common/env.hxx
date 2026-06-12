#pragma once

#include <filesystem>
#include <string>

namespace rcb{
	
/* Env is only called once early as part of init. */
/* For a library separation, each command class will have it's own env object in its parameters. Like execve(2) */

struct Env
{
	static constexpr const char* kEVar {"RCB_DIR"};

	static constexpr const char* kRoot {".rcb"};
	static constexpr const char* kFile {"file"};
	static constexpr const char* kData {"data"};
	static constexpr const char* kWipe {"wipe"};
	static constexpr const char* kSign {"sign"};

	std::filesystem::path rootDir{};
	std::filesystem::path fileDir{};
	std::filesystem::path dataDir{};
	std::filesystem::path wipeDir{};
	std::filesystem::path signDir{};
	std::string_view      ownerID{};
};

class EnvResolver
{
public:
	EnvResolver();
	Env resolve();
	
private:
	std::filesystem::path m_rootDir{};
	std::filesystem::path m_fileDir{};
	std::filesystem::path m_dataDir{};
	std::filesystem::path m_wipeDir{};
	std::filesystem::path m_signDir{}; 
	std::filesystem::path m_confDir{};
	std::string           m_ownerID{};
	
	void var();
	void dir();
	void conf();
	//void setSharedEnv();
	// void initEnv();
};

} // namespace rcb
