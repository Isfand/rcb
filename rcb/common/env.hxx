#pragma once
#include <filesystem>
#ifndef ENV_HXX
#define ENV_HXX

#include <string>

namespace rcb{

/*Env is only called once early as part of init.*/

class Env
{
public:
	Env();
	
private:

	std::filesystem::path m_workingProgDir{};
	std::filesystem::path m_workingProgFileDir{};
	std::filesystem::path m_workingProgDataDir{};
	std::filesystem::path m_workingProgWipeDir{};
	std::filesystem::path m_workingProgWordDir{}; 
	std::filesystem::path m_workingConfDir{};
	std::string m_workingUsername{};
	
	void var();
	void dir();
	void conf();
	void setSharedEnv();
	// void initEnv();
};

} // namespace rcb

#endif // ENV_HXX
