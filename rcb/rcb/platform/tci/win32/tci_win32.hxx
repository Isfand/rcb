#if defined(_WIN32) && !defined(__CYGWIN__)

#pragma once
#ifndef TCI_WIN32_HXX
#define TCI_WIN32_HXX

namespace tci{

struct Termsize
{
	unsigned short row;
	unsigned short col;
	Termsize();
};

}//namespace tci

#endif //TCI_WIN32_HXX

#endif //#if defined(_WIN32) && !defined(__CYGWIN__)