#pragma once
#ifndef DELETE_ARGS_HXX
#define DELETE_ARGS_HXX

#include <string>
#include <vector>

namespace rcb{

struct DeleteOptions
{
	bool verboseOption{};
	bool forceOption{};
	bool silentOption{};
	bool noDirSizeOption{};
};

class DeleteArgs
{
public:
	DeleteArgs(std::vector<std::string>& args);
	void run(std::vector<std::string>& args);
private:
	std::vector<int> m_erasePositions{};
	DeleteOptions m_dOpt;

	const char* m_deleteHelpMsg =
		"delete usage:\n" 
		"delete <filename> | d <filename>, erase file by filename\n"
		"--help            | -h \n"
		"--verbose         | -v \n"
		"--silent          | -s \n"
		"--force           | -f \n"
		"--no-directorysize, don't save size for filetype:directory\n"
		"--, options end after this point.\n"
		;
};

} // namespace rcb

#endif // DELETE_ARGS_HXX
