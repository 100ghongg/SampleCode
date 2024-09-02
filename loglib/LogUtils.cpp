#include "LogUtils.h"
#include <stdarg.h>
#include <sys/time.h>

namespace LogUtils
{
    void CLogWriter::SetLogOutput(uint type)
    {
		logfuncs_.clear();

		if (type & static_cast<uint>(LogType::StdOut)) {
			logfuncs_.push_back( [] (const std::string& logMsg) { std::cout << logMsg << '\n'; } );
		}

		if (type & static_cast<uint>(LogType::File) && logfile_.is_open()) {
			logfuncs_.push_back( [&] (const std::string& logMsg) { logfile_ << logMsg; logfile_.flush(); } );
		}

		if (type & static_cast<uint>(LogType::DB) && logDB_) {
			logfuncs_.push_back( [&] (const std::string& logMsg) { logDB_->WriteLog(logMsg); } );
		}
    }

	CLogWriter::~CLogWriter()
	{
		if (logDB_) {
			logDB_->Disconnect();
		}
		if (logfile_.is_open()) {
			logfile_.close();
		}
	}

	void Logging(const std::string& logMsg)
	{
		g_LogWriter->Logging(logMsg);
	}

	thread_local char temp[max_loglen];

    std::string GetCurTimeString()
    {
		struct timeval ass;
		auto now = std::chrono::system_clock::now();
		time_t curTmt = std::chrono::system_clock::to_time_t(now);
    	struct tm stTm;
	#if defined(__linux__) || defined(__unix__)
		localtime_r(&curTmt, &stTm);
	#elif defined(WIN32) || defined(__WIN32)
		localtime_s(&stTm, &curTmt);
	#endif
    	char tmp[128] = { 0 };
    	snprintf(tmp, sizeof(tmp), "%04d-%02d-%02d %02d:%02d:%02d.%03ld", stTm.tm_year + 1900, stTm.tm_mon + 1, stTm.tm_mday, stTm.tm_hour, stTm.tm_min, stTm.tm_sec, std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000);

    	return std::string(tmp);
	}

	void DebugLog(const char* logMsg, ...)
	{
		// Varg
		ZeroMemory(temp, max_loglen);
		va_list args;
		va_start(args, logMsg);
    #if defined(__linux__) || defined(__unix__)
        vsnprintf(temp, max_loglen, logMsg, args);
    #elif defined(WIN32) || defined(__WIN32)
		vsnprintf_s(temp, max_loglen, max_loglen, logMsg, args);
    #endif
		va_end(args);
		std::string logstr(temp);

		// Logging
		Logging( AddDefaultInfo(LogLevel::Debug, logstr));
	}

	void DebugLogDetail(const char* fileName, const char* funcName, long lineNum, const char* logMsg, ...)
	{
		// Varg
		ZeroMemory(temp, max_loglen);
		va_list args;
		va_start(args, logMsg);
    #if defined(__linux__) || defined(__unix__)
        vsnprintf(temp, max_loglen, logMsg, args);
    #elif defined(WIN32) || defined(__WIN32)
		len = vsprintf_s(nullptr, 0, logMsg, args);
    #endif
		va_end(args);
		std::string logstr(temp);

		// Add Details
		logstr = logstr + " [" + funcName + "][" + fileName + ':' + std::to_string(lineNum) + ']';

		// Logging
		Logging( AddDefaultInfo(LogLevel::Debug, logstr));
	}

    void InfoLog(const char *logMsg, ...)
    {
		// Varg
		ZeroMemory(temp, max_loglen);
		va_list args;
		va_start(args, logMsg);
    #if defined(__linux__) || defined(__unix__)
        vsnprintf(temp, max_loglen, logMsg, args);
    #elif defined(WIN32) || defined(__WIN32)
		vsnprintf_s(temp, max_loglen, max_loglen, logMsg, args);
    #endif
		va_end(args);
		std::string logstr(temp);

		// Logging
		Logging( AddDefaultInfo(LogLevel::Info, logstr));
    }

    void InfoLogDetail(const char *fileName, const char *funcName, long lineNum, const char *logMsg, ...)
    {
		// Varg
		ZeroMemory(temp, max_loglen);
		va_list args;
		va_start(args, logMsg);
    #if defined(__linux__) || defined(__unix__)
        vsnprintf(temp, max_loglen, logMsg, args);
    #elif defined(WIN32) || defined(__WIN32)
		len = vsprintf_s(nullptr, 0, logMsg, args);
    #endif
		va_end(args);
		
		std::string logstr(temp);

		// Add Details
		logstr = logstr + " [" + funcName + "][" + fileName + ':' + std::to_string(lineNum) + ']';

		// Logging
		Logging( AddDefaultInfo(LogLevel::Info, logstr));
    }

    void FailLog(const char *logMsg, ...)
    {
		// Varg
		ZeroMemory(temp, max_loglen);
		va_list args;
		va_start(args, logMsg);
    #if defined(__linux__) || defined(__unix__)
        vsnprintf(temp, max_loglen, logMsg, args);
    #elif defined(WIN32) || defined(__WIN32)
		vsnprintf_s(temp, max_loglen, max_loglen, logMsg, args);
    #endif
		va_end(args);

		std::string logstr(temp);
		// logstr = std::string( temp );

		// Logging
		Logging( AddDefaultInfo(LogLevel::Fail, logstr));
    }

    void FailLogDetail(const char *fileName, const char *funcName, long lineNum, const char *logMsg, ...)
    {
		// Varg
		ZeroMemory(temp, max_loglen);
		va_list args;
		va_start(args, logMsg);
    #if defined(__linux__) || defined(__unix__)
        vsnprintf(temp, max_loglen, logMsg, args);
    #elif defined(WIN32) || defined(__WIN32)
		len = vsprintf_s(nullptr, 0, logMsg, args);
    #endif
		va_end(args);
		
		std::string logstr(temp);

		// Add Details
		logstr = logstr + " [" + funcName + "][" + fileName + ':' + std::to_string(lineNum) + ']';

		// Logging
		Logging( AddDefaultInfo(LogLevel::Fail, logstr));
    }

    void ErrorLog(const char *logMsg, ...)
    {
		// Varg
		ZeroMemory(temp, max_loglen);
		va_list args;
		va_start(args, logMsg);
    #if defined(__linux__) || defined(__unix__)
        vsnprintf(temp, max_loglen, logMsg, args);
    #elif defined(WIN32) || defined(__WIN32)
		vsnprintf_s(temp, max_loglen, max_loglen, logMsg, args);
    #endif
		va_end(args);
		
		std::string logstr(temp);

		// Logging
		Logging( AddDefaultInfo(LogLevel::Error, logstr));
    }

    void ErrorLogDetail(const char *fileName, const char *funcName, long lineNum, const char *logMsg, ...)
    {
		// Varg
		ZeroMemory(temp, max_loglen);
		va_list args;
		va_start(args, logMsg);
    #if defined(__linux__) || defined(__unix__)
        vsnprintf(temp, max_loglen, logMsg, args);
    #elif defined(WIN32) || defined(__WIN32)
		len = vsprintf_s(nullptr, 0, logMsg, args);
    #endif
		va_end(args);

		std::string logstr(temp);

		// Add Details
		logstr = logstr + " [" + funcName + "][" + fileName + ':' + std::to_string(lineNum) + ']';

		// Logging
		Logging( AddDefaultInfo(LogLevel::Error, logstr));
    }

    std::string& AddDefaultInfo(LogLevel level, std::string& logMsg)
	{
		// Log level
		std::string levelString;
		switch (level)
		{
		case LogUtils::LogLevel::Debug:
			levelString = "Debug";
			break;
		case LogUtils::LogLevel::Info:
			levelString = "Info";
			break;
		case LogUtils::LogLevel::Fail:
			levelString = "Fail";
			break;
		case LogUtils::LogLevel::Error:
			levelString = "Error";
			break;
		default:
			levelString = "Invalid Log Level";
			break;
		}

		logMsg = '[' + levelString + "] " + logMsg + " [" + GetCurTimeString() + ']';

		return logMsg;
	}
}
