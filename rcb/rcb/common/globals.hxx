#pragma once

#include "singleton.hxx"

namespace rcb{

// Project
inline constexpr const char* g_kProgName        {"rcb"};
inline constexpr const char* g_kProgVersion     {"0.15.0"};

// Singleton
inline Singleton* g_singleton = Singleton::instance();

} // namespace rcb