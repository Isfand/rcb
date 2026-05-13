#pragma once
#ifndef PLATFORM_TCI_HXX
#define PLATFORM_TCI_HXX

#if defined(__unix__) || defined(__APPLE__)
#include "posix/tci_posix.hxx"
#elif defined(_WIN32) && !defined(__CYGWIN__)
#include "win32/tci_win32.hxx"
#endif

//Terminal Control Interface
namespace tci{

using Termsize = Termsize;

}//namespace tci

#endif //PLATFORM_TCI_HXX
