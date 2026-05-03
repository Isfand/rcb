#pragma once
#ifndef GLOBAL_HXX
#define GLOBAL_HXX

#include "singleton.hxx"

namespace rcb{

//Project
inline const char* g_kProgName{"rcb"};
inline const char* g_kProgVersion{"0.13.0"};

//Env
inline const char* g_kEnvFile {"file"};
inline const char* g_kEnvData {"data"};
inline const char* g_kEnvWipe {"wipe"};
inline const char* g_kEnvSign {"sign"};

//Singleton
inline Singleton* g_singleton = Singleton::instance();

//Database
inline const std::string g_kDatabaseName {std::string(g_kProgName) + ".sqlite3"}; //{"rcb.sqlite3"}; //Database filename
inline const char* g_kSchemaID           {"id"};            //SQL auto increment
inline const char* g_kSchemaFile         {"file"};          //Refers to filename to be inside of .rcb/file/
inline const char* g_kSchemaPath         {"path"};          //Complete filesystem path
inline const char* g_kSchemaTimestamp    {"timestamp"};     //Unix Timestamp since epoch
inline const char* g_kSchemaSize         {"size"};          //Bytes
inline const char* g_kSchemaFiletype     {"filetype"};      //Filesystem file type
inline const char* g_kSchemaPathDepth    {"depth"};         //Path depth
inline const char* g_kSchemaUser         {"user"};          //The user who executed the command
inline const char* g_kSchemaExecutionID  {"execution"};     //Execution id of all files deleted in the same run

} //namespace rcb

#endif //GLOBAL_HXX