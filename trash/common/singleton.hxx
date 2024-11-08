#pragma once
#ifndef SINGLETON_HXX
#define SINGLETON_HXX

#include <string>
#include <mutex>

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

	const std::string getWorkingTrashDir()     const;
	const std::string getWorkingTrashFileDir() const;
	const std::string getWorkingTrashDataDir() const;
	const std::string getWorkingTrashWipeDir() const;
	const std::string getWorkingTrashWordDir() const;
	const std::string getWorkingUsername()     const;

	//Mutators
	//void setVerboseOption      (bool verboseOption);
	//void setForceOption        (bool forceOption);

	void setWorkingTrashDir    (std::string workingTrashDir);
	void setWorkingTrashFileDir(std::string workingTrashFileDir);
	void setWorkingTrashDataDir(std::string workingTrashDataDir);
	void setWorkingTrashWipeDir(std::string workingTrashWipeDir);
	void setWorkingTrashWordDir(std::string workingTrashWordDir);
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
	
	std::string m_workingTrashDir{};
	std::string m_workingTrashFileDir{};
	std::string m_workingTrashDataDir{};
	std::string m_workingTrashWipeDir{};
	std::string m_workingTrashWordDir{}; 
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
