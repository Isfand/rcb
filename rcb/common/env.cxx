#include <filesystem>
#include <print>
#include <stdexcept>

#include "env.hxx"
#include "globals.hxx"

//TEMPORARY: USE ACI in future.
#if defined(__unix__) || defined(__APPLE__)
const char* HOME = "HOME";
const char* USER = "USER";
#elif defined(_WIN32) && !defined(__CYGWIN__)
const char* HOME = "USERPROFILE";
const char* USER = "USERNAME";
#endif

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
	//FUTURE:
	//TODO. Optimize this code to use init if statement. Ideally all connected.
	
	if(std::getenv(HOME) == NULL)
	{
		throw std::runtime_error("$HOME is NULL");
	} // Prevent segmentation fault
	else if(std::getenv(USER) == NULL)
	{
		throw std::runtime_error("$USER is NULL");
	}

	m_workingUsername = (std::string(std::getenv(USER)));
		
	if(std::getenv("RCB_DIR") == NULL) // Prevent segmentation fault
		m_workingProgDir = (std::filesystem::path(std::getenv(HOME)) / (std::string(".") + g_progName)).string();
	else
		m_workingProgDir = (std::string(std::getenv("RCB_DIR")));
}

void Env::dir()
{
	// TODO. Create a config to set dir and internal names that way it can work for any deviated implementation
	// Prog internal dir names
	const char* file {"file"};
	const char* data {"data"};
	const char* wipe {"wipe"};
	const char* word {"word"};

	m_workingProgFileDir = m_workingProgDir / file;
	m_workingProgDataDir = m_workingProgDir / data;
	m_workingProgWipeDir = m_workingProgDir / wipe;
	m_workingProgWordDir = m_workingProgDir / word;

#ifndef NDEBUG
	std::println("{0} working directory is: {1}", g_progName, m_workingProgDir.string());
#endif

	//Check if active dir exists. If not then execute the below.
	if(!std::filesystem::exists(m_workingProgFileDir) || 
	   !std::filesystem::exists(m_workingProgDataDir) || 
	   !std::filesystem::exists(m_workingProgWipeDir) ||
	   !std::filesystem::exists(m_workingProgWordDir))
	{
		for (const auto& dir : {m_workingProgFileDir, 
								m_workingProgDataDir, 
								m_workingProgWipeDir, 
								m_workingProgWordDir})
								std::filesystem::create_directories(dir);
		
#ifndef NDEBUG
		std::println("Directories {0},{1},{2},{3} Created in: {4}", file, data, wipe, word, m_workingProgDir.string());
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
	g_singleton->setWorkingProgWordDir(m_workingProgWordDir);
	g_singleton->setWorkingUsername   (m_workingUsername);
	//Add conf env
}

} // namespace rcb
