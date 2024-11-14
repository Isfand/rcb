#include <iostream>

#include <filesystem>

#include "singleton.hxx"

namespace trash{

Singleton* Singleton::s_instance = nullptr;
std::once_flag Singleton::s_flag;

//std::once_flag Singleton::s_setVerboseOptionFlag;
//std::once_flag Singleton::s_setForceOptionFlag;

std::once_flag Singleton::s_setWorkingTrashDirFlag;
std::once_flag Singleton::s_setWorkingTrashFileDirFlag;
std::once_flag Singleton::s_setWorkingTrashDataDirFlag;
std::once_flag Singleton::s_setWorkingTrashWipeDirFlag;
std::once_flag Singleton::s_setWorkingTrashWordDirFlag;
std::once_flag Singleton::s_setWorkingUsernameFlag;

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

//Accessors. Get many.
//const bool Singleton::getVerboseOption() const
//{
//	return m_verboseOption;
//}
//const bool Singleton::getForceOption() const
//{
//	return m_forceOption;
//}
const std::filesystem::path Singleton::getWorkingTrashDir() const
{
	return m_workingTrashDir;
}
const std::filesystem::path Singleton::getWorkingTrashFileDir() const
{
	return m_workingTrashFileDir;
}
const std::filesystem::path Singleton::getWorkingTrashDataDir() const
{
	return m_workingTrashDataDir;
}
const std::filesystem::path Singleton::getWorkingTrashWipeDir() const
{
	return m_workingTrashWipeDir;
}
const std::filesystem::path Singleton::getWorkingTrashWordDir() const
{
	return m_workingTrashWordDir;
}
const std::string Singleton::getWorkingUsername() const
{
	return m_workingUsername;
}

//Mutators. Set once.
//TODO. Maybe add a runtime or compile time check. E.G Throw exception or static_assert.
//void Singleton::setVerboseOption(bool verboseOption)
//{
//	std::call_once(s_setVerboseOptionFlag, [&](){ m_verboseOption = verboseOption;});
//}
//void Singleton::setForceOption(bool forceOption)
//{
//	std::call_once(s_setForceOptionFlag, [&](){ m_forceOption = forceOption;});
//}
void Singleton::setWorkingTrashDir(std::filesystem::path workingTrashDir)
{
	std::call_once(s_setWorkingTrashDirFlag, [&](){ m_workingTrashDir = workingTrashDir;});
}
void Singleton::setWorkingTrashFileDir(std::filesystem::path workingTrashFileDir)
{
	std::call_once(s_setWorkingTrashFileDirFlag, [&](){ m_workingTrashFileDir = workingTrashFileDir;});
}
void Singleton::setWorkingTrashDataDir(std::filesystem::path workingTrashDataDir)
{
	std::call_once(s_setWorkingTrashDataDirFlag, [&](){ m_workingTrashDataDir = workingTrashDataDir;});
}
void Singleton::setWorkingTrashWipeDir(std::filesystem::path workingTrashWipeDir)
{
	std::call_once(s_setWorkingTrashWipeDirFlag, [&](){ m_workingTrashWipeDir = workingTrashWipeDir;});
}
void Singleton::setWorkingTrashWordDir(std::filesystem::path workingTrashWordDir)
{
	std::call_once(s_setWorkingTrashWordDirFlag, [&](){ m_workingTrashWordDir = workingTrashWordDir;});
}
void Singleton::setWorkingUsername(std::string workingUsername)
{
	std::call_once(s_setWorkingUsernameFlag, [&](){ m_workingUsername = workingUsername;});
}

// Singleton* inst;

} //namespace trash
