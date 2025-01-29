#pragma once
#ifndef PLATFORM_ACI_HXX
#define PLATFORM_ACI_HXX

#if defined(__unix__) || defined(__APPLE__)
#include "posix/posix.hxx"
#elif defined(_WIN32) && !defined(__CYGWIN__)
#include "windows/windows.hxx"
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

}//namespace aci

#endif //PLATFORM_ACI_HXX
