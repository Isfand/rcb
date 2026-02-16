#pragma once
#ifndef GLOBAL_HXX
#define GLOBAL_HXX

#include "singleton.hxx"

namespace rcb{

//Project
inline const char* g_kProgName{"rcb"};
inline const char* g_kProgVersion{"0.12.3"};

//Singleton
inline Singleton* g_singleton = Singleton::instance();

} //namespace rcb

#endif //GLOBAL_HXX