#pragma once
#ifndef STOPWATCH_HXX
#define STOPWATCH_HXX

// Debug macros for shorter syntax
//#define FILE_FUNC_NAME std::string(__FILE__) + " in " + std::string(__func__)

#include <chrono>

namespace rcb {
	
class Stopwatch
{
public:
	Stopwatch();
	~Stopwatch();
	void start();
	void stop();
	void reset();
	long long elapsedMilliseconds();
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time, m_end_time;
	bool m_running;  
};

} //namespace rcb
#endif //STOPWATCH_HXX
