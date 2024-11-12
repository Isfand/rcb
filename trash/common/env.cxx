#include <filesystem>
#include <print>
#include <stdexcept>

#include "env.hxx"
#include "globals.hxx"

//TEMPORARY: Move to ACI.
#if defined(__unix__) || defined(__APPLE__)
const char* HOME = "HOME";
const char* USER = "USER";
#elif defined(_WIN32) && !defined(__CYGWIN__)
const char* HOME = "USERPROFILE";
const char* USER = "USERNAME";
#endif

namespace trash{

Env::Env()
{
	envVar();
	envDir();
	envConf();
	setSharedEnv();
}

void Env::envVar()
{
	//FUTURE:
	//TODO. Optimize this code to use init if statement. Ideally all connected.
	//TODO. use pw_homedir instead of getenv as it is faster without lookup and it's intention is to be platform agnostic.
	//TODO. Instead use platform/aci.hxx for the UID and pwnam to get USER.
	
	if(std::getenv(HOME) == NULL)
	{
		throw std::runtime_error("$HOME is NULL");
	} // Prevent segmentation fault
	else if(std::getenv(USER) == NULL)
	{
		throw std::runtime_error("$USER is NULL");
	}

	m_workingUsername = (std::string(std::getenv(USER)));
		
	if(std::getenv("TRASH_DIR") == NULL) // Prevent segmentation fault
		m_workingTrashDir = (std::string(std::getenv(HOME)) + "/." + g_progName + "/");
	else
		m_workingTrashDir = (std::string(std::getenv("TRASH_DIR")));
	//WARNING: windows uses backslash \ instead of forward slash / for directories. Both are compatable. But mixing can be an issue.

}

void Env::envDir()
{
	// TODO. Create a config to set dir and internal names that way it can work for any deviated implementation
	// Trash internal dir names
	const char* file {"file/"};
	const char* data {"data/"};
	const char* wipe {"wipe/"};
	const char* word {"word/"};

	m_workingTrashFileDir = m_workingTrashDir + file;
	m_workingTrashDataDir = m_workingTrashDir + data;
	m_workingTrashWipeDir = m_workingTrashDir + wipe;
	m_workingTrashWordDir = m_workingTrashDir + word;

#ifndef NDEBUG
	std::println("{0} working directory is: {1}", g_progName, m_workingTrashDir);
#endif

	//Check if active dir exists. If not then execute the below.
	if(!std::filesystem::exists(m_workingTrashFileDir) || 
	   !std::filesystem::exists(m_workingTrashDataDir) || 
	   !std::filesystem::exists(m_workingTrashWipeDir) ||
	   !std::filesystem::exists(m_workingTrashWordDir))
	{
		// Reduce to one line. If possible.
		//std::filesystem::create_directories(m_workingTrashFileDir);
		//std::filesystem::create_directories(m_workingTrashDataDir);
		//std::filesystem::create_directories(m_workingTrashWipeDir);
		//std::filesystem::create_directories(m_workingTrashWordDir);

		for (const auto& dir : {m_workingTrashFileDir, 
								m_workingTrashDataDir, 
								m_workingTrashWipeDir, 
								m_workingTrashWordDir})
								std::filesystem::create_directories(dir);
		
#ifndef NDEBUG
		std::println("Directories {0},{1},{2},{3} Created in: {4}", file, data, wipe, word, m_workingTrashDir);
#endif
	}
}

void Env::envConf()
{
	// Implement when needed
}

void Env::setSharedEnv()
{
	singleton->setWorkingTrashDir    (m_workingTrashDir);
	singleton->setWorkingTrashFileDir(m_workingTrashFileDir);
	singleton->setWorkingTrashDataDir(m_workingTrashDataDir);
	singleton->setWorkingTrashWipeDir(m_workingTrashWipeDir);
	singleton->setWorkingTrashWordDir(m_workingTrashWordDir);
	singleton->setWorkingUsername    (m_workingUsername);
	//Add conf env
}

// void Env::initEnv()
// {
//     envVar();
//     envDir();
//     envConf();
// }

} // namespace trash
