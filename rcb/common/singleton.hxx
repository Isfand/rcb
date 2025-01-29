#pragma once
#ifndef SINGLETON_HXX
#define SINGLETON_HXX

#include <string>
#include <mutex>
#include <filesystem>

namespace rcb{

//Using alteration of Blair Davidson's singleton: https://loosechainsaw.github.io/c++/2020/02/16/singleton/
class Singleton
{
public:
	static Singleton* instance();
	
	//https://www.stroustrup.com/C++11FAQ.html#default2
	Singleton(const Singleton&)            = delete; // Prevent copy ctor
	Singleton& operator=(const Singleton&) = delete; // Prevent copy assignment
	Singleton(Singleton&&)                 = delete; // Prevent move ctor
	Singleton& operator=(Singleton&&)      = delete; // Prevent move assignment
	~Singleton();

	//Accessors
	//const bool        getVerboseOption()       const;
	//const bool        getForceOption()         const;

	const std::filesystem::path getWorkingProgDir()     const;
	const std::filesystem::path getWorkingProgFileDir() const;
	const std::filesystem::path getWorkingProgDataDir() const;
	const std::filesystem::path getWorkingProgWipeDir() const;
	const std::filesystem::path getWorkingProgWordDir() const;
	const std::string getWorkingUsername()               const;

	//Mutators
	//void setVerboseOption      (bool verboseOption);
	//void setForceOption        (bool forceOption);

	void setWorkingProgDir    (std::filesystem::path workingProgDir);
	void setWorkingProgFileDir(std::filesystem::path workingProgFileDir);
	void setWorkingProgDataDir(std::filesystem::path workingProgDataDir);
	void setWorkingProgWipeDir(std::filesystem::path workingProgWipeDir);
	void setWorkingProgWordDir(std::filesystem::path workingProgWordDir);
	void setWorkingUsername   (std::string workingProgUsername);

	//Assign
	// const bool&& g_kVerboseFlag = getVerboseFlag();
	
private:
	Singleton();
	//int m_state = 1; //Unused

	static Singleton *s_instance;
	//Do NOT inline these once_flags. avoid multiple definitions and potential issues with thread synchronization
	static std::once_flag s_flag;

	//static std::once_flag s_setVerboseOptionFlag;
	//static std::once_flag s_setForceOptionFlag;

	static std::once_flag s_setWorkingProgDirFlag;
	static std::once_flag s_setWorkingProgFileDirFlag;
	static std::once_flag s_setWorkingProgDataDirFlag;
	static std::once_flag s_setWorkingProgWipeDirFlag;
	static std::once_flag s_setWorkingProgWordDirFlag;
	static std::once_flag s_setWorkingUsernameFlag;

	//bool m_verboseOption{};
	//bool m_forceOption{};
	
	std::filesystem::path m_workingProgDir{};
	std::filesystem::path m_workingProgFileDir{};
	std::filesystem::path m_workingProgDataDir{};
	std::filesystem::path m_workingProgWipeDir{};
	std::filesystem::path m_workingProgWordDir{}; 
	std::string m_workingUsername{};
};

//Unused
template<typename T>
class ScopedSingletonDeleter {
public:
	explicit ScopedSingletonDeleter(T* ptr)
		: m_ptr_{ptr}
	{
	}
	ScopedSingletonDeleter(ScopedSingletonDeleter const&)            = delete;
	ScopedSingletonDeleter& operator=(const ScopedSingletonDeleter&) = delete;
	ScopedSingletonDeleter(ScopedSingletonDeleter&&)                 = delete;
	ScopedSingletonDeleter& operator=(ScopedSingletonDeleter&&)      = delete;
	~ScopedSingletonDeleter()
	{
		delete m_ptr_;
		m_ptr_ = nullptr;
	}
private:
	T* m_ptr_{};
};

} //namespace rcb

#endif //SINGLETON_HXX
