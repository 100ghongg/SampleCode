#pragma once
#include "Default.h"
#include "DBUtils.h"
#include <source_location>

/*
*  ** Log Format
*  Default Log format
*  [LogLevel] Log messages ... [2024-12-31 12:34.56]
* 
*  Detail Info Added 
*  [LogLevel] Log messages ... [2024-12-31 12:34.56][FuncName][FileName:lineNum]
* 
*  ** Log Level
*  0. Debug. OutputDebugString
*  1. Info
*  2. Fail
*  3. Error
*
*  ** Code Usage
*  1. Macro Type
*    LogUtils::LOGLEVEL(LogMsg)    ex)LogUtils::DEBUG("Ghong log %d, %c", 1234, 'c');
*  2. Class Type
*    glog.Loglevel(LogMsg)         ex)glog.Debug("Ghong log %d, %c", 1234, 'c');
*
*/

////////////////// 
// Config

// Log Level
#define _LOGLEVEL_DEBUG_ 0
#define _LOGLEVEL_INFO_  1
#define _LOGLEVEL_FAIL_  2
#define _LOGLEVEL_ERROR_ 3

// Add Detail Info([FuncName][FailName:Line] ) to logs
#define __DetailLogLevel__ _LOGLEVEL_FAIL_

// Log Message Length Limit
#define __MaxLogLen__ 2048

// Usage Style
// Macro Style : LogUtils::DEBUG("Logmsg %d%d", 1,2)
#define _LOG_USAGE_MACRO
// Class Style : log.debug("Logmsg %d%d", 1,2)
// require : __cplusplus > 201703L
#define _LOG_USAGE_CLASS

// Output type(stdout/file/db)
#define _LOG_OUT_STD     1
#define _LOG_OUT_FILE    0
#define _LOG_OUT_DB      0

// End Config
//-----------------------------------------------------------

// Delete the full path from __FILENAME__
#define _LINUX_PATH_SLASH_ '/'
#define _WINDOW_PATH_SLASH_ '\\'
#if defined(__linux__) || defined(__unix__)
    #define _OS_PATH_SLASH_ _LINUX_PATH_SLASH_
#elif defined(WIN32) || defined(__WIN32)
    #define _OS_PATH_SLASH_ _WINDOW_PATH_SLASH_
    // #define __FILENAME__ (strrchr(__FILE__, _WINDOW_PATH_SLASH_)+1)
#endif
#define __FILENAME__ (strrchr(__FILE__, _OS_PATH_SLASH_)+1)

namespace DBUtils{ class CLogDatabase; }

namespace LogUtils
{
	enum class LogLevel	{ Debug, Info, Fail, Error };
    enum class LogType { StdOut = 0x001, File = 0x002, DB = 0x004 };
    constexpr uint   default_logout_type = (_LOG_OUT_STD) | ((_LOG_OUT_FILE)<<1) | ((_LOG_OUT_DB) << 2);
	constexpr size_t max_loglen = __MaxLogLen__;

    // LogWriter - write logs according to Output type
    class CLogWriter
    {
    private:
        DBUtils::CLogDatabase*   logDB_{nullptr};
        std::ofstream            logfile_;
        std::vector<std::function<void (const std::string& logMsg)>> logfuncs_;
        
    public:
	    inline void  Logging(const std::string& logMsg) {
            for( auto& fn : logfuncs_ )
                fn(logMsg);
        };
        void    SetLogOutput(uint type);
        CLogWriter() { SetLogOutput(default_logout_type); };
        ~CLogWriter();
    };
    typedef Singleton<CLogWriter>      LogWriterSingleton;
    #define g_LogWriter                LogWriterSingleton::getInstance()

    // Logging functions
    std::string GetCurTimeString();
    void DebugLog(const char* logMsg, ...);
    void DebugLogDetail(const char* fileName, const char* funcName, long lineNum, const char* logMsg, ...);
    void InfoLog(const char* logMsg, ...);
    void InfoLogDetail(const char* fileName, const char* funcName, long lineNum, const char* logMsg, ...);
    void FailLog(const char* logMsg, ...);
    void FailLogDetail(const char* fileName, const char* funcName, long lineNum, const char* logMsg, ...);
    void ErrorLog(const char* logMsg, ...);
    void ErrorLogDetail(const char* fileName, const char* funcName, long lineNum, const char* logMsg, ...);
	std::string& AddDefaultInfo(LogLevel level, std::string& logMsg);
    void Logging(const std::string& logMsg);

///////////////////////////
// For Class style Usage
// 
#if defined(_LOG_USAGE_CLASS)
    struct LogCaller
    {
        char tmp_log[max_loglen];
        std::source_location loc_info;
        LogCaller(const std::source_location& loc = std::source_location::current()) : loc_info(loc) {};
        template<typename... Types>
        void Debug(const char* logMsg, Types... args)
        {
            ZeroMemory(tmp_log, max_loglen);
            sprintf(tmp_log, logMsg, args...);

        #if __DetailLogLevel__ >= _LOGLEVEL_DEBUG_
            DebugLogDetail( (strrchr(loc_info.file_name(), _OS_PATH_SLASH_ )+1), loc_info.function_name() , loc_info.line(), tmp_log);
        #else
            DebugLog(tmp_log);
        #endif  // _LOGLEVEL_DEBUG_
        }
        template<typename... Types>
        void Info(const char* logMsg, Types... args)
        {
            ZeroMemory(tmp_log, max_loglen);
            sprintf(tmp_log, logMsg, args...);

        #if __DetailLogLevel__ >= _LOGLEVEL_INFO_
            InfoLogDetail( (strrchr(loc_info.file_name(), _OS_PATH_SLASH_ )+1), loc_info.function_name() , loc_info.line(), tmp_log);
        #else
            InfoLog(tmp_log);
        #endif  // _LOGLEVEL_DEBUG_
        }
        template<typename... Types>
        void Fail(const char* logMsg, Types... args)
        {
            ZeroMemory(tmp_log, max_loglen);
            sprintf(tmp_log, logMsg, args...);

        #if __DetailLogLevel__ >= _LOGLEVEL_FAIL_
            FailLogDetail( (strrchr(loc_info.file_name(), _OS_PATH_SLASH_ )+1), loc_info.function_name() , loc_info.line(), tmp_log);
        #else
            FailLog(tmp_log);
        #endif  // _LOGLEVEL_DEBUG_
        }
        template<typename... Types>
        void Error(const char* logMsg, Types... args)
        {
            ZeroMemory(tmp_log, max_loglen);
            sprintf(tmp_log, logMsg, args...);
        #if __DetailLogLevel__ >= _LOGLEVEL_ERROR_
            ErrorLogDetail( (strrchr(loc_info.file_name(), _OS_PATH_SLASH_ )+1), loc_info.function_name() , loc_info.line(), tmp_log);
        #else
            ErrorLog(tmp_log);
        #endif  // _LOGLEVEL_DEBUG_
        }
    };
#endif  // _LOG_USAGE_CLASS
}

#if defined(_LOG_USAGE_CLASS)
    #define glog LogUtils::LogCaller{}
#endif  // _LOG_USAGE_CLASS


///////////////////////////
// For Macro style Usage
// 
#if defined(_LOG_USAGE_MACRO)
    #if __DetailLogLevel__ >= _LOGLEVEL_DEBUG_
        #define DEBUG(...) DebugLogDetail(__FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #else
        #define DEBUG(...) DebugLog(__VA_ARGS__)
    #endif  // _LOGLEVEL_DEBUG_

    #if __DetailLogLevel__ >= _LOGLEVEL_INFO_
        #define INFO(...) InfoLogDetail(__FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #else
        #define INFO(...) InfoLog(__VA_ARGS__)
    #endif  // _LOGLEVEL_INFO_

    #if __DetailLogLevel__ >= _LOGLEVEL_FAIL_
        #define FAIL(...) FailLogDetail(__FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #else
        #define FAIL(...) FailLog(__VA_ARGS__)
    #endif  // _LOGLEVEL_FAIL_

    // Always Add details to Error Log
    #if __DetailLogLevel__ // >= _LOGLEVEL_ERROR_
        #define ERROR(...) ErrorLogDetail(__FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__)
    #else
        #define ERROR(...) ErrorLog(__VA_ARGS__)
    #endif  // _LOGLEVEL_ERROR_
#endif  // _LOG_USAGE_MACRO
