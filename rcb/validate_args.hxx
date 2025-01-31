#pragma once
#ifndef VALIDATE_ARGS_HXX
#define VALIDATE_ARGS_HXX

#include <string>
#include <vector>

namespace rcb{

struct ValidateOptions
{
	bool verboseOption{};
	bool yesOption{};
	bool silentOption{};
	bool fileOption{};
	bool dataOption{};
	bool wipeOption{};
	bool fillDirSizeOption{};
};

class ValidateArgs
{
public:
	ValidateArgs(std::vector<std::string>& args);
private:
	ValidateOptions m_vOpt;
	
	void run(std::vector<std::string>& args);

	const char* m_validateHelpMsg = 
		"validate usage:\n"
		"--help    | -h\n"
		"--verbose | -v\n"
		"--yes     | -y\n"
		"--all     | -a, validate all directories.\n"
		"--file    | -F, validate file/ only\n"
		"--data    | -D, validate data/ only\n"
		"--wipe    | -W, validate wipe/ only\n"
		"--fill-directorysize, validate size:NULL of filetype:directory\n"
		;
};
	
}//namespace rcb

#endif
