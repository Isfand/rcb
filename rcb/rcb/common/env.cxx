#include <filesystem>
#include <print>
#include <stdexcept>

#include "env.hxx"
#include "globals.hxx"
#include "rcb/platform/aci/aci.hxx"


namespace rcb{
	
Env::Env()
{
	var();
	dir();
	conf();
	setSharedEnv();
}

void Env::var()
{
	const std::string home = aci::Pwuid().pw_dir();
	const std::string user = aci::Pwuid().pw_name();

	//FUTURE:
	//TODO. Optimize this code to use init if statement. Ideally all connected.
	if(home.empty())
	{
		throw std::runtime_error("HOME NOT FOUND");
	} // Prevent segmentation fault
	else if(user.empty())
	{
		throw std::runtime_error("USER NOT FOUND");
	}

	m_workingUsername = user;
		
	if(std::getenv("RCB_DIR") == NULL) // Prevent segmentation fault
		m_workingProgDir = (std::filesystem::path(home) / (std::string(".") + g_kProgName)).string();
	else
		m_workingProgDir = (std::string(std::getenv("RCB_DIR")));
}

void Env::dir()
{
	m_workingProgFileDir = m_workingProgDir / g_kEnvFile;
	m_workingProgDataDir = m_workingProgDir / g_kEnvData;
	m_workingProgWipeDir = m_workingProgDir / g_kEnvWipe;
	m_workingProgSignDir = m_workingProgDir / g_kEnvSign;

#ifndef NDEBUG
	std::println("{0} working directory is: {1}", g_kProgName, m_workingProgDir.string());
#endif

	//Check if active dir exists. If not then execute the below.
	if(!std::filesystem::exists(m_workingProgFileDir) || 
	   !std::filesystem::exists(m_workingProgDataDir) || 
	   !std::filesystem::exists(m_workingProgWipeDir) ||
	   !std::filesystem::exists(m_workingProgSignDir))
	{
		for (const auto& dir : {m_workingProgFileDir, 
								m_workingProgDataDir, 
								m_workingProgWipeDir, 
								m_workingProgSignDir})
								std::filesystem::create_directories(dir);
		
#ifndef NDEBUG
		std::println("Directories {0},{1},{2},{3} Created in: {4}", g_kEnvFile, g_kEnvData, g_kEnvWipe, g_kEnvSign, m_workingProgDir.string());
#endif
	}
}

void Env::conf()
{
	// Implement when needed
}

void Env::setSharedEnv()
{
	g_singleton->setWorkingProgDir    (m_workingProgDir);
	g_singleton->setWorkingProgFileDir(m_workingProgFileDir);
	g_singleton->setWorkingProgDataDir(m_workingProgDataDir);
	g_singleton->setWorkingProgWipeDir(m_workingProgWipeDir);
	g_singleton->setWorkingProgSignDir(m_workingProgSignDir);
	g_singleton->setWorkingUsername   (m_workingUsername);
	//Add conf env
}

} // namespace rcb
