#include <filesystem>
#include <print>
#include <stdexcept>

#include "env.hxx"
#include "rcb/platform/aci/aci.hxx"


namespace rcb{
	
EnvResolver::EnvResolver()
{
	var();
	dir();
	conf();
	//setSharedEnv();
}

void EnvResolver::var()
{
	const std::string home = aci::Pwuid().pw_dir();
	const std::string user = aci::Pwuid().pw_name();

	// FUTURE:
	// TODO. Optimize this code to use init if statement. Ideally all connected.
	if(home.empty())
	{
		throw std::runtime_error("HOME NOT FOUND");
	} // Prevent segmentation fault
	else if(user.empty())
	{
		throw std::runtime_error("USER NOT FOUND");
	}

	m_ownerID = user;
		
	if(std::getenv(Env::kEVar) == NULL) // Prevent segmentation fault
		m_rootDir = (std::filesystem::path(home) / Env::kRoot).string();
	else
		m_rootDir = (std::string(std::getenv(Env::kEVar)));
}

void EnvResolver::dir()
{
	m_fileDir = m_rootDir / Env::kFile;
	m_dataDir = m_rootDir / Env::kData;
	m_wipeDir = m_rootDir / Env::kWipe;
	m_signDir = m_rootDir / Env::kSign;

#ifndef NDEBUG
	std::println("{0} working directory is: {1}", Env::kRoot, m_rootDir.string());
#endif

	// Check if active dir exists. If not then execute the below.
	if(!std::filesystem::exists(m_fileDir) || 
	   !std::filesystem::exists(m_dataDir) || 
	   !std::filesystem::exists(m_wipeDir) ||
	   !std::filesystem::exists(m_signDir))
	{
		for (const auto& dir : {m_fileDir, 
								m_dataDir, 
								m_wipeDir, 
								m_signDir})
								std::filesystem::create_directories(dir);
		
#ifndef NDEBUG
		std::println("Directories {0},{1},{2},{3} Created in: {4}", Env::kFile, Env::kData, Env::kWipe, Env::kSign, m_rootDir.string());
#endif
	}
}

void EnvResolver::conf()
{
	// Implement when needed
}

/*
void EnvResolver::setSharedEnv()
{
	g_singleton->setWorkingProgDir    (m_workingProgDir);
	g_singleton->setWorkingProgFileDir(m_workingProgFileDir);
	g_singleton->setWorkingProgDataDir(m_workingProgDataDir);
	g_singleton->setWorkingProgWipeDir(m_workingProgWipeDir);
	g_singleton->setWorkingProgSignDir(m_workingProgSignDir);
	g_singleton->setWorkingUsername   (m_workingUsername);
	// Add conf env
}
*/

Env EnvResolver::resolve()
{
	return Env
	{ 
		.rootDir = m_rootDir,
		.fileDir = m_fileDir,
		.dataDir = m_dataDir,
		.wipeDir = m_wipeDir,
		.signDir = m_signDir,
		.ownerID = m_ownerID
	};
}

} // namespace rcb
