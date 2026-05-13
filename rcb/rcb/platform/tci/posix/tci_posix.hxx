#if defined(__unix__) || defined(__APPLE__)

#pragma once
#ifndef TCI_POSIX_HXX
#define TCI_POSIX_HXX

namespace tci{

struct Termsize
{
	unsigned short row;
	unsigned short col;
	Termsize();
};

}//namespace tci

#endif //TCI_POSIX_HXX

#endif //#if defined(__unix__) || defined(__APPLE__)