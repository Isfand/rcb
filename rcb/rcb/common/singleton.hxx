#pragma once

#include <string>
#include <mutex>
#include <filesystem>

namespace rcb{

// Using alteration of Blair Davidson's singleton: https:// loosechainsaw.github.io/c++/2020/02/16/singleton/
class Singleton
{
public:
	static Singleton* instance();
	
	// https:// www.stroustrup.com/C++11FAQ.html#default2
	Singleton(const Singleton&)            = delete; // Prevent copy ctor
	Singleton& operator=(const Singleton&) = delete; // Prevent copy assignment
	Singleton(Singleton&&)                 = delete; // Prevent move ctor
	Singleton& operator=(Singleton&&)      = delete; // Prevent move assignment
	~Singleton();

	// Accessors
	// const bool        getVerboseOption()       const;
	// const bool        getForceOption()         const;

	// Could make these return std::optional
	const std::filesystem::path getRootDir() const;
	const std::filesystem::path getFileDir() const;
	const std::filesystem::path getDataDir() const;
	const std::filesystem::path getWipeDir() const;
	const std::filesystem::path getSignDir() const;
	const std::string           getOwnerID() const;

	// Mutators
	// void setVerboseOption      (bool verboseOption);
	// void setForceOption        (bool forceOption);

	void setRootDir(std::filesystem::path rootDir);
	void setFileDir(std::filesystem::path fileDir);
	void setDataDir(std::filesystem::path dataDir);
	void setWipeDir(std::filesystem::path wipeDir);
	void setSignDir(std::filesystem::path signDir);
	void setOwnerID(std::string           ownerID);

	// Assign
	// const bool&& g_kVerboseFlag = getVerboseFlag();
	
private:
	Singleton();
	// int m_state = 1; // Unused
	// NOTE: These are set to static which means they will persist even after object recreation, which can be a problem for tests. An alternative solution for debugging could be later created.
	static Singleton *s_instance;
	// Do NOT inline these once_flags. avoid multiple definitions and potential issues with thread synchronization
	static std::once_flag s_flag;

	// static std::once_flag s_setVerboseOptionFlag;
	// static std::once_flag s_setForceOptionFlag;

	static std::once_flag s_setRootDirFlag;
	static std::once_flag s_setFileDirFlag;
	static std::once_flag s_setDataDirFlag;
	static std::once_flag s_setWipeDirFlag;
	static std::once_flag s_setSignDirFlag;
	static std::once_flag s_setOwnerIDFlag;

	// bool m_verboseOption{};
	// bool m_forceOption{};
	
	std::filesystem::path m_rootDir{};
	std::filesystem::path m_fileDir{};
	std::filesystem::path m_dataDir{};
	std::filesystem::path m_wipeDir{};
	std::filesystem::path m_signDir{}; 
	std::string           m_ownerID{};
};

// Unused
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

} // namespace rcb
