#pragma once

#include <filesystem>
#include <string>

namespace rcb{
	
/* Env is only called once early as part of init. */
/* For a library separation, each command class will have it's own env object in its parameters. Like execve(2) */

inline constexpr const char* g_kEnvFile {"file"};
inline constexpr const char* g_kEnvData {"data"};
inline constexpr const char* g_kEnvWipe {"wipe"};
inline constexpr const char* g_kEnvSign {"sign"};

class Env
{
public:
	Env();
	
private:
	std::filesystem::path m_workingProgDir{};
	std::filesystem::path m_workingProgFileDir{};
	std::filesystem::path m_workingProgDataDir{};
	std::filesystem::path m_workingProgWipeDir{};
	std::filesystem::path m_workingProgSignDir{}; 
	std::filesystem::path m_workingConfDir{};
	std::string           m_workingUsername{};
	
	void var();
	void dir();
	void conf();
	void setSharedEnv();
	// void initEnv();
};

} // namespace rcb
