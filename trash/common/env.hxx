#pragma once
#include <filesystem>
#ifndef ENV_HXX
#define ENV_HXX

#include <string>

namespace trash{

/*Env is only called once early as part of init.*/

class Env
{
public:
	Env();
	
private:

	std::filesystem::path m_workingTrashDir{};
	std::filesystem::path m_workingTrashFileDir{};
	std::filesystem::path m_workingTrashDataDir{};
	std::filesystem::path m_workingTrashWipeDir{};
	std::filesystem::path m_workingTrashWordDir{}; 
	std::filesystem::path m_workingConfDir{};
	std::string m_workingUsername{};
	
	void var();
	void dir();
	void conf();
	void setSharedEnv();
	// void initEnv();
};

} // namespace trash

#endif // ENV_HXX
