#include <iostream>
#include <cstdlib>
#include <exception>
#include <print>

#include "args.hxx"
#ifndef NDEBUG
#include "common/stopwatch.hxx"
#endif//NDEBUG

//#include "common/singleton.hxx"
//#include "common/globals.hxx"

using namespace trash;

int main (int argc, char** argv)
{
#ifndef NDEBUG
	Stopwatch stopwatch;
#endif
	// Run
	Args args(argc, argv); 

	try
	{
		args.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		//ScopedSingletonDeleter<Singleton> _{singleton};
		return EXIT_FAILURE;
	}
	//ScopedSingletonDeleter<Singleton> _{singleton};
	return EXIT_SUCCESS;
}
