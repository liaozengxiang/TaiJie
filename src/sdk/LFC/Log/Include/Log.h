/*
 * 定义应用程序缺省的日志，有系统日志和运行日志
 * 若应用程序想定义更多的日志，可以从 CLog 派生出子类，并编写对应的宏输出日志
 */

#ifndef ___LOG__H__20160511___
#define ___LOG__H__20160511___

#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/loggingmacros.h>
#include <Common/OSType.h>
#include <Common/Singleton.h>
#include <string>

class CLog
{
    DECLARE_SINGLETON(CLog)

protected:
    CLog();
    virtual ~CLog();

public:
    /*
     * 功能: 初始化日志对象
     * 参数: 
     *      @lpszSysLog     -- 系统日志文件名，文件名必须可写
     *      @nSysFileSize   -- 系统日志文件大小
     *      @nSysFileCount  -- 系统日志文件个数
     *      @lpszRunLog     -- 运行日志文件名，文件名必须可写
     *      @nRunFileSize   -- 运行日志文件大小
     *      @nRunFileCount  -- 运行日志文件个数
     *      @bConsole       -- 是否在控制台同步显示日志
     */
    Bool Initialize(LPCSTR lpszSysLog, Int32 nSysFileSize, Int32 nSysFileCount,
                    LPCSTR lpszRunLog, Int32 nRunFileSize, Int32 nRunFileCount, Bool bConsole);
    void SetSysLogLevel(Int32 nLevel);
    void SetRunLogLevel(Int32 nLevel);
    log4cplus::Logger& GetSysLogger();
    log4cplus::Logger& GetRunLogger();

protected:
    log4cplus::Logger                  m_sysLogger;
    log4cplus::Logger                  m_runLogger;
};

#define TRACE_LOG(msg) \
    do \
    { \
        LOG4CPLUS_TRACE(CSingleton<CLog>::GetInstancePtr()->GetSysLogger(), msg); \
    } while (0)

#define DEBUG_LOG(msg) \
    do \
    { \
        LOG4CPLUS_DEBUG(CSingleton<CLog>::GetInstancePtr()->GetSysLogger(), msg); \
    } while (0)

#define INFO_LOG(msg) \
    do \
    { \
        LOG4CPLUS_INFO(CSingleton<CLog>::GetInstancePtr()->GetSysLogger(), msg); \
    } while (0)

#define WARN_LOG(msg) \
    do \
    { \
        LOG4CPLUS_WARN(CSingleton<CLog>::GetInstancePtr()->GetSysLogger(), msg); \
    } while (0)

#define ERROR_LOG(msg) \
    do \
    { \
        LOG4CPLUS_ERROR(CSingleton<CLog>::GetInstancePtr()->GetSysLogger(), msg); \
    } while (0)

#define FATAL_LOG(msg) \
    do \
    { \
        LOG4CPLUS_FATAL(CSingleton<CLog>::GetInstancePtr()->GetSysLogger(), msg); \
    } while (0)

#define RUN_TRACE_LOG(msg) \
    do \
    { \
        LOG4CPLUS_TRACE(CSingleton<CLog>::GetInstancePtr()->GetRunLogger(), msg); \
    } while (0)

#define RUN_DEBUG_LOG(msg) \
    do \
    { \
        LOG4CPLUS_DEBUG(CSingleton<CLog>::GetInstancePtr()->GetRunLogger(), msg); \
    } while (0)

#define RUN_INFO_LOG(msg) \
    do \
    { \
        LOG4CPLUS_INFO(CSingleton<CLog>::GetInstancePtr()->GetRunLogger(), msg); \
    } while (0)

#define RUN_WARN_LOG(msg) \
    do \
    { \
        LOG4CPLUS_WARN(CSingleton<CLog>::GetInstancePtr()->GetRunLogger(), msg); \
    } while (0)

#define RUN_ERROR_LOG(msg) \
    do \
    { \
        LOG4CPLUS_ERROR(CSingleton<CLog>::GetInstancePtr()->GetRunLogger(), msg); \
    } while (0)

#define RUN_FATAL_LOG(msg) \
    do \
    { \
        LOG4CPLUS_FATAL(CSingleton<CLog>::GetInstancePtr()->GetRunLogger(), msg); \
    } while (0)

#endif
