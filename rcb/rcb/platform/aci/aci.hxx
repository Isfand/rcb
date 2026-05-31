#pragma once

// TODO: Use the PIMPL design pattern or a forward declaration to hide this header to prevent C pollution.
// Or just wait for C++ Modules to be fully implemented.
#if defined(__unix__) || defined(__APPLE__)
#include "posix/aci_posix.hxx"
#include "posix/faccessat_posix.hxx"
#elif defined(_WIN32) && !defined(__CYGWIN__)
#include "win32/aci_win32.hxx"
#include "win32/faccessat_win32.hxx"
#endif

// Access Control Interface
namespace aci{
 
using Stat      = Stat;
using Chown     = Chown;
using Utime     = Utime;
// using Utimensat = Utimensat;
using User      = User;
using Pwuid     = Pwuid;
using Pwnam     = Pwnam;


// WIP: Do NOT use.
using Faccessat = Faccessat;

// TODO: add isatty() from unistd.h. 
// Need to know if the program is called in a terminal or is being piped. Pass in kernel fds

using Termsize  = Termsize;

}// namespace aci
