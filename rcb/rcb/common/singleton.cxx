#include <iostream>

#include <filesystem>

#include "singleton.hxx"

namespace rcb{

Singleton* Singleton::s_instance = nullptr;
std::once_flag Singleton::s_flag;

//std::once_flag Singleton::s_setVerboseOptionFlag;
//std::once_flag Singleton::s_setForceOptionFlag;

std::once_flag Singleton::s_setWorkingProgDirFlag;
std::once_flag Singleton::s_setWorkingProgFileDirFlag;
std::once_flag Singleton::s_setWorkingProgDataDirFlag;
std::once_flag Singleton::s_setWorkingProgWipeDirFlag;
std::once_flag Singleton::s_setWorkingProgSignDirFlag;
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
const std::filesystem::path Singleton::getWorkingProgDir() const
{
	return m_workingProgDir;
}
const std::filesystem::path Singleton::getWorkingProgFileDir() const
{
	return m_workingProgFileDir;
}
const std::filesystem::path Singleton::getWorkingProgDataDir() const
{
	return m_workingProgDataDir;
}
const std::filesystem::path Singleton::getWorkingProgWipeDir() const
{
	return m_workingProgWipeDir;
}
const std::filesystem::path Singleton::getWorkingProgSignDir() const
{
	return m_workingProgSignDir;
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
void Singleton::setWorkingProgDir(std::filesystem::path workingProgDir)
{
	std::call_once(s_setWorkingProgDirFlag, [&](){ m_workingProgDir = workingProgDir;});
}
void Singleton::setWorkingProgFileDir(std::filesystem::path workingProgFileDir)
{
	std::call_once(s_setWorkingProgFileDirFlag, [&](){ m_workingProgFileDir = workingProgFileDir;});
}
void Singleton::setWorkingProgDataDir(std::filesystem::path workingProgDataDir)
{
	std::call_once(s_setWorkingProgDataDirFlag, [&](){ m_workingProgDataDir = workingProgDataDir;});
}
void Singleton::setWorkingProgWipeDir(std::filesystem::path workingProgWipeDir)
{
	std::call_once(s_setWorkingProgWipeDirFlag, [&](){ m_workingProgWipeDir = workingProgWipeDir;});
}
void Singleton::setWorkingProgSignDir(std::filesystem::path workingProgSignDir)
{
	std::call_once(s_setWorkingProgSignDirFlag, [&](){ m_workingProgSignDir = workingProgSignDir;});
}
void Singleton::setWorkingUsername(std::string workingUsername)
{
	std::call_once(s_setWorkingUsernameFlag, [&](){ m_workingUsername = workingUsername;});
}

// Singleton* inst;

} //namespace rcb
