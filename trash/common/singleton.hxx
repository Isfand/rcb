#pragma once
#ifndef SINGLETON_HXX
#define SINGLETON_HXX

#include <string>
#include <mutex>
#include <filesystem>

namespace trash{

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

	const std::filesystem::path getWorkingTrashDir()     const;
	const std::filesystem::path getWorkingTrashFileDir() const;
	const std::filesystem::path getWorkingTrashDataDir() const;
	const std::filesystem::path getWorkingTrashWipeDir() const;
	const std::filesystem::path getWorkingTrashWordDir() const;
	const std::string getWorkingUsername()               const;

	//Mutators
	//void setVerboseOption      (bool verboseOption);
	//void setForceOption        (bool forceOption);

	void setWorkingTrashDir    (std::filesystem::path workingTrashDir);
	void setWorkingTrashFileDir(std::filesystem::path workingTrashFileDir);
	void setWorkingTrashDataDir(std::filesystem::path workingTrashDataDir);
	void setWorkingTrashWipeDir(std::filesystem::path workingTrashWipeDir);
	void setWorkingTrashWordDir(std::filesystem::path workingTrashWordDir);
	void setWorkingUsername    (std::string workingTrashUsername);

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

	static std::once_flag s_setWorkingTrashDirFlag;
	static std::once_flag s_setWorkingTrashFileDirFlag;
	static std::once_flag s_setWorkingTrashDataDirFlag;
	static std::once_flag s_setWorkingTrashWipeDirFlag;
	static std::once_flag s_setWorkingTrashWordDirFlag;
	static std::once_flag s_setWorkingUsernameFlag;

	//bool m_verboseOption{};
	//bool m_forceOption{};
	
	std::filesystem::path m_workingTrashDir{};
	std::filesystem::path m_workingTrashFileDir{};
	std::filesystem::path m_workingTrashDataDir{};
	std::filesystem::path m_workingTrashWipeDir{};
	std::filesystem::path m_workingTrashWordDir{}; 
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

} //namespace trash

#endif //SINGLETON_HXX
