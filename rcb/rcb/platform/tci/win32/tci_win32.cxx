#if defined(_WIN32) && !defined(__CYGWIN__)

#include <windows.h>
#include <iostream>

#include "tci_win32.hxx"

namespace tci{

Termsize::Termsize()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	if (!GetConsoleScreenBufferInfo(h, &csbi))
	{
		std::cerr << "GetConsoleScreenBufferInfo failed\n";
		this->row = 0;
		this->col = 0;
		return; //return 1;
	}
	//return 0;
	this->row = csbi.srWindow.Bottom - csbi.srWindow.Top  + 1;
	this->col = csbi.srWindow.Right  - csbi.srWindow.Left + 1;
}

}//namespace tci

#endif //#if defined(_WIN32) && !defined(__CYGWIN__)