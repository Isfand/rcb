#pragma once
#ifndef GLOBAL_HXX
#define GLOBAL_HXX

#include "singleton.hxx"

namespace rcb{

//Project
inline constexpr const char* g_kProgName{"rcb"};
inline constexpr const char* g_kProgVersion{"0.13.0"};

//Env: Currently only used in env.cxx
inline constexpr const char* g_kEnvFile {"file"};
inline constexpr const char* g_kEnvData {"data"};
inline constexpr const char* g_kEnvWipe {"wipe"};
inline constexpr const char* g_kEnvSign {"sign"};

//Database: WARNING Do NOT change these. Doing so will break compatibility with existing tables with previous schema.
inline const std::string g_kDatabaseName {std::string(g_kProgName) + ".sqlite3"}; //{"rcb.sqlite3"}; //Database filename
inline constexpr const char* g_kSchemaID        {"id"};        //SQL auto increment
inline constexpr const char* g_kSchemaFile      {"file"};      //Refers to filename to be inside of .rcb/file/
inline constexpr const char* g_kSchemaPath      {"path"};      //Complete filesystem path
inline constexpr const char* g_kSchemaTimestamp {"timestamp"}; //Unix Timestamp since epoch
inline constexpr const char* g_kSchemaSize      {"size"};      //Bytes
inline constexpr const char* g_kSchemaFiletype  {"filetype"};  //Filesystem file type
inline constexpr const char* g_kSchemaPathDepth {"depth"};     //Path depth
inline constexpr const char* g_kSchemaUser      {"user"};      //The user who executed the command
inline constexpr const char* g_kSchemaExecution {"execution"}; //Execution id of all files deleted in the same run

//Singleton
inline Singleton* g_singleton = Singleton::instance();

} //namespace rcb

#endif //GLOBAL_HXX