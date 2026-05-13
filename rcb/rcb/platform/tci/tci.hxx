#pragma once
#ifndef PLATFORM_TCI_HXX
#define PLATFORM_TCI_HXX

//TODO: Use the PIMPL design pattern or a forward declaration to hide this header to prevent C pollution.
//Or just wait for C++ Modules to be fully implemented.
//#if defined(__unix__) || defined(__APPLE__)
//#include "posix/tci_posix.hxx"
//#elif defined(_WIN32) && !defined(__CYGWIN__)
//#include "win32/tci_win32.hxx"
//#endif

//Terminal Control Interface
namespace tci{

//Create a portable wrapper for getting terminal height & width.
//sys/ioctl.h
//Windows.h
 


}//namespace tci

#endif //PLATFORM_TCI_HXX
