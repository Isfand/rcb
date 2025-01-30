#pragma once
#ifndef ARGS_HXX
#define ARGS_HXX

#include <vector>
#include <string>

namespace rcb{

class Args
{
public:
	Args(int argc, char** argv);

	enum class Command
	{
		Delete,
		Erase,
		List,
		Restore,
		Validate
	};
		
	void run();
		
private:
	int m_argc;
	char** m_argv;

	//Globals to be set in setGlobals();
	//Always pushback globals in Args to m_argsToRemove
	bool m_verboseOption  {}; //Incomplete
	bool m_forceOption    {}; //Incomplete
	//bool m_silentOption   {}; //Unused
	//int  m_threadOptional {}; //Unused

	//bool m_tactacFlag{}; //Unused
	//bool m_tacFlag   {}; //Unused
	
	Command m_cmd{-1}; // Shared flag for primary commands. Sentinel Value
	
	bool m_sharedCmdFlag    {}; // sharedGlobFlag exists to save time to not do string search index if false
	bool m_sharedGlobFlag   {}; 
	bool m_initFinishedFlag {}; //Checks if init function has reached the end of its scope and not done an early exit.

	std::vector<std::string> m_args{};
	std::vector<std::string> m_argsToRemove{};

	void init();
	void deGlob();
	void setSharedOptions();
	void runCmd();


	const char* m_helpMsg =
		"Primary Commands:\n"
		"delete   | d, deletes files.\n"
		"list     | l, lists files.\n"
		"restore  | r, restores files.\n"
		"erase    | e, empty files.\n"
		"validate | v, validate files.\n\n"

		"Shared Options:\n"
		"--help   | -h, gives help info of the program and a primary command when paired.\n\n"

		"Top-Level Options:\n"
		"--version, gives program version.\n";

	//https://www.asciiart.eu/logos/recycle
	const char* m_rcb_ASCII_art =
	R"(
    _____\    _______
   /      \  |      /\
  /_______/  |_____/  \
 |   \   /        /   /
  \   \         \/   /
   \  /          \__/_  
    \/ ____    /\    
      /  \    /  \   
     /\   \  /   /   
       \   \/   /    
        \___\__/
    )";
};

} // namespace rcb

#endif // ARGS_HXX
