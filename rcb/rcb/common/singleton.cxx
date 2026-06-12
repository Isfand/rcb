#include <iostream>
#include <string>
#include <filesystem>

#include "singleton.hxx"

namespace rcb{

Singleton* Singleton::s_instance = nullptr;

// Note: consider using atomic<bool> for deallocated recreations
std::once_flag Singleton::s_flag;

// std::once_flag Singleton::s_setVerboseOptionFlag;
// std::once_flag Singleton::s_setForceOptionFlag;

std::once_flag Singleton::s_setRootDirFlag;
std::once_flag Singleton::s_setFileDirFlag;
std::once_flag Singleton::s_setDataDirFlag;
std::once_flag Singleton::s_setWipeDirFlag;
std::once_flag Singleton::s_setSignDirFlag;
std::once_flag Singleton::s_setOwnerIDFlag;

Singleton* Singleton::instance()
{
	std::call_once(s_flag, []{ s_instance = new Singleton; });
	return s_instance;
}

Singleton::Singleton()
{
#ifndef NDEBUG
	std::cout << "Singleton()" << std::endl;
#endif
}

Singleton::~Singleton()
{
#ifndef NDEBUG
	std::cout << "~Singleton()" << std::endl;
#endif
}

// Accessors. Get many.
// const bool Singleton::getVerboseOption() const
//{
//	return m_verboseOption;
//}
// const bool Singleton::getForceOption() const
//{
//	return m_forceOption;
//}
const std::filesystem::path Singleton::getRootDir() const
{
	return m_rootDir;
}
const std::filesystem::path Singleton::getFileDir() const
{
	return m_fileDir;
}
const std::filesystem::path Singleton::getDataDir() const
{
	return m_dataDir;
}
const std::filesystem::path Singleton::getWipeDir() const
{
	return m_wipeDir;
}
const std::filesystem::path Singleton::getSignDir() const
{
	return m_signDir;
}
const std::string Singleton::getOwnerID() const
{
	return m_ownerID;
}

// Mutators. Set once.
// TODO. Maybe add a runtime or compile time check. E.G Throw exception or static_assert.
// void Singleton::setVerboseOption(bool verboseOption)
//{
//	std::call_once(s_setVerboseOptionFlag, [&](){ m_verboseOption = verboseOption;});
//}
// void Singleton::setForceOption(bool forceOption)
//{
//	std::call_once(s_setForceOptionFlag, [&](){ m_forceOption = forceOption;});
//}
void Singleton::setRootDir(std::filesystem::path workingProgDir)
{
	std::call_once(s_setRootDirFlag, [&](){ m_rootDir = workingProgDir;});
}
void Singleton::setFileDir(std::filesystem::path workingProgFileDir)
{
	std::call_once(s_setFileDirFlag, [&](){ m_fileDir = workingProgFileDir;});
}
void Singleton::setDataDir(std::filesystem::path workingProgDataDir)
{
	std::call_once(s_setDataDirFlag, [&](){ m_dataDir = workingProgDataDir;});
}
void Singleton::setWipeDir(std::filesystem::path workingProgWipeDir)
{
	std::call_once(s_setWipeDirFlag, [&](){ m_wipeDir = workingProgWipeDir;});
}
void Singleton::setSignDir(std::filesystem::path workingProgSignDir)
{
	std::call_once(s_setSignDirFlag, [&](){ m_signDir = workingProgSignDir;});
}
void Singleton::setOwnerID(std::string workingUsername)
{
	std::call_once(s_setOwnerIDFlag, [&](){ m_ownerID = workingUsername;});
}

// Singleton* inst;

} // namespace rcb
