#pragma once
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

	std::string m_workingTrashDir{};
	std::string m_workingTrashFileDir{};
	std::string m_workingTrashDataDir{};
	std::string m_workingTrashWipeDir{};
	std::string m_workingTrashWordDir{}; 
	std::string m_workingUsername{};
	std::string m_workingConfDir{};
	
	void var();
	void dir();
	void conf();
	void setSharedEnv();
	// void initEnv();
};

} // namespace trash

#endif // ENV_HXX
