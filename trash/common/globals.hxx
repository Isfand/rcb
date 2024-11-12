#pragma once
#ifndef GLOBAL_HXX
#define GLOBAL_HXX

#include "singleton.hxx"

namespace trash{

//Project
inline const char* g_progName{"trash"};
inline const char* g_progVersion{"0.9.1"};

//Singleton
inline Singleton* singleton = Singleton::instance();

} //namespace trash

#endif //GLOBAL_HXX