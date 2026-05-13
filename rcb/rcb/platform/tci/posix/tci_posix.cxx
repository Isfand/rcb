#if defined(__unix__) || defined(__APPLE__)

#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

#include "tci_posix.hxx"

namespace tci{


termsize::termsize()
{
	winsize w;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
	{
		std::cerr << "ioctl failed\n";
		this->row = 0;
		this->col = 0;
		return; //return 1;
	}
	//return 0;

	this->col = w.ws_col;
	this->row = w.ws_row;
}


}//namespace tci

#endif //#if defined(__unix__) || defined(__APPLE__)