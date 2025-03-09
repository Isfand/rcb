#pragma once
#ifndef GLOBAL_HXX
#define GLOBAL_HXX

#include "singleton.hxx"

namespace rcb{

//Project
inline const char* g_progName{"rcb"};
inline const char* g_progVersion{"0.12.2"};

//Singleton
inline Singleton* g_singleton = Singleton::instance();

} //namespace rcb

#endif //GLOBAL_HXX