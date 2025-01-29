#include <chrono>
#include <print>

#include "stopwatch.hxx"

namespace rcb{

Stopwatch::Stopwatch(): m_running(false)
{
	start();
}

Stopwatch::~Stopwatch()
{
	stop();
	std::println("{} microseconds",elapsedMilliseconds());
};

// Starts the stopwatch
void Stopwatch::start() 
{
	if (!m_running) 
	{
		m_start_time = std::chrono::high_resolution_clock::now();
		m_running = true;
	}
}

// Stops the stopwatch
void Stopwatch::stop()
{
	if (m_running) 
	{
		m_end_time = std::chrono::high_resolution_clock::now();
		m_running = false;
	}
}

// Resets the stopwatch
void Stopwatch::reset() 
{
	m_running = false;
}

// Returns the elapsed time in milliseconds
long long Stopwatch::elapsedMilliseconds() 
{
	if (m_running) 
	{
		auto now = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(now - m_start_time).count();
	} 
	else 
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(m_end_time - m_start_time).count();
	}
}

} //namespace rcb

