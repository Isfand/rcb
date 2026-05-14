#pragma once
#ifndef PLATFORM_ACI_HXX
#define PLATFORM_ACI_HXX

//TODO: Use the PIMPL design pattern or a forward declaration to hide this header to prevent C pollution.
//Or just wait for C++ Modules to be fully implemented.
#if defined(__unix__) || defined(__APPLE__)
#include "posix/aci_posix.hxx"
#elif defined(_WIN32) && !defined(__CYGWIN__)
#include "win32/aci_win32.hxx"
#endif

//Access Control Interface
namespace aci{
 
using Stat      = Stat;
using Chown     = Chown;
using Utime     = Utime;
//using Utimensat = Utimensat;
using User      = User;
using Pwuid     = Pwuid;
using Pwnam     = Pwnam;

using Termsize = Termsize;

//TODO: add isatty() from unistd.h. 
//Need to know if the program is called in a terminal or is being piped. Pass in kernel fds

}//namespace aci

#endif //PLATFORM_ACI_HXX
