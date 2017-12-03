#include "../Include/Log.h"
#include <time.h>
#include <Common/OS.h>

CLog::CLog()
{
    // 创建缺省的日志对象，防止未初始化时 core dump
    m_sysLogger = log4cplus::Logger::getInstance("UnInitedDefaultLogger");
    m_runLogger = m_sysLogger;
}

CLog::~CLog()
{
}

Bool CLog::Initialize(LPCSTR lpszSysLog, Int32 nSysFileSize, Int32 nSysFileCount, LPCSTR lpszRunLog, Int32 nRunFileSize, Int32 nRunFileCount, Bool bConsole)
{
    // 创建 Layout
    std::auto_ptr<log4cplus::Layout> sysFileLayout(new log4cplus::PatternLayout("%D{%x %X.%q}\t%p\t%m\t[%l]%n"));
    std::auto_ptr<log4cplus::Layout> runFileLayout(new log4cplus::PatternLayout("%D{%x %X.%q}\t%p\t%m%n"));

    // 创建 Appender
    log4cplus::SharedAppenderPtr spSysLogFileAppender(new log4cplus::RollingFileAppender(lpszSysLog, nSysFileSize, nSysFileCount));
    log4cplus::SharedAppenderPtr spRunLogFileAppender(new log4cplus::RollingFileAppender(lpszRunLog, nRunFileSize, nRunFileCount));

    // 设置文件格式
    spSysLogFileAppender->setLayout(sysFileLayout);
    spRunLogFileAppender->setLayout(runFileLayout);

    // 创建 Logger 对象
    m_sysLogger = log4cplus::Logger::getInstance("syslog");
    m_runLogger = log4cplus::Logger::getInstance("runlog");

    m_sysLogger.addAppender(spSysLogFileAppender);
    m_runLogger.addAppender(spRunLogFileAppender);

    if (bConsole)
    {
        std::auto_ptr<log4cplus::Layout> sysConsoleLayout(new log4cplus::PatternLayout("%D{%x %X.%q}\t%p\t%m\t[%l]%n"));
        std::auto_ptr<log4cplus::Layout> runConsoleLayout(new log4cplus::PatternLayout("%D{%x %X.%q}\t%p\t%m%n"));

        log4cplus::SharedAppenderPtr spSysLogConsoleAppender(new log4cplus::ConsoleAppender());
        log4cplus::SharedAppenderPtr spRunLogConsoleAppender(new log4cplus::ConsoleAppender());
        spSysLogConsoleAppender->setLayout(sysConsoleLayout);
        spRunLogConsoleAppender->setLayout(runConsoleLayout);
        m_sysLogger.addAppender(spSysLogConsoleAppender);
        m_runLogger.addAppender(spRunLogConsoleAppender);
    }

    return True;
}

void CLog::SetSysLogLevel(Int32 nLevel)
{
    log4cplus::LogLevel eLevel;
    switch (nLevel)
    {
    case 0:
        eLevel = log4cplus::TRACE_LOG_LEVEL;
        break;

    case 1:
        eLevel = log4cplus::DEBUG_LOG_LEVEL;
        break;

    case 2:
        eLevel = log4cplus::INFO_LOG_LEVEL;
        break;

    case 3:
        eLevel = log4cplus::WARN_LOG_LEVEL;
        break;

    case 4:
        eLevel = log4cplus::ERROR_LOG_LEVEL;
        break;

    case 5:
        eLevel = log4cplus::FATAL_LOG_LEVEL;
        break;

    case 6:
        eLevel = log4cplus::OFF_LOG_LEVEL;
        break;

    default:
        eLevel = log4cplus::TRACE_LOG_LEVEL;
        break;
    }

    m_sysLogger.setLogLevel(eLevel);
}

void CLog::SetRunLogLevel(Int32 nLevel)
{
    log4cplus::LogLevel eLevel;
    switch (nLevel)
    {
    case 0:
        eLevel = log4cplus::TRACE_LOG_LEVEL;
        break;

    case 1:
        eLevel = log4cplus::DEBUG_LOG_LEVEL;
        break;

    case 2:
        eLevel = log4cplus::INFO_LOG_LEVEL;
        break;

    case 3:
        eLevel = log4cplus::WARN_LOG_LEVEL;
        break;

    case 4:
        eLevel = log4cplus::ERROR_LOG_LEVEL;
        break;

    case 5:
        eLevel = log4cplus::FATAL_LOG_LEVEL;
        break;

    case 6:
        eLevel = log4cplus::OFF_LOG_LEVEL;
        break;

    default:
        eLevel = log4cplus::TRACE_LOG_LEVEL;
        break;
    }

    m_runLogger.setLogLevel(eLevel);
}

log4cplus::Logger& CLog::GetSysLogger()
{
    return m_sysLogger;
}

log4cplus::Logger& CLog::GetRunLogger()
{
    return m_runLogger;
}
