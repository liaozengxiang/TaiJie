/*
 * 封装与操作系统相关的函数，使接口统一
 *
 */

#ifndef ___OS__H__20140418___
#define ___OS__H__20140418___

#include <string>
#include <list>
#include <signal.h>
#include "OSType.h"

namespace OS
{
	Bool Initialize();														// 初始化函数
	Bool InitDaemon();														// 将当前进程设置为后台进程，并将标准输入/标准输出/标准错误重定向到 /dev/null
	void RegisterSignal(Int32 nSignal, void (*SigFunc)(Int32 nSignal));		// 注册信号的处理函数
	void SetProcessTitle(LPCSTR lpszTitle, Int32 argc, char *argv[]);		// 修改进程标题（设置标题后 argv 参数将失效）

	Int32 GetCoreCount();													// 获取 CPU 核心的数量
	Bool  SetThreadAffinity(Int32 nCPUIndex, Int32 nThreadID = 0);			// 设置线程与 CPU 绑定

	void  Sleep(Int32 nMillisecond);										// 毫秒级休眠
	Int64 Milliseconds();													// 获取当前时间的毫秒值
    Int64 GetTickCount();													// 获取自系统启动后的毫秒数

    Bool  SetResourceLimit(Int32 nResource, Int64 lCur, Int64 lMax);        // 设置资源限制
	Bool  SetMaxFDSize(Int64 lSize);										// 设置系统最大的文件数
    Bool  SetCoreDumpSize(Int64 lSize);                                     // 设置 core dump 大小
	Int32 GetLastError();													// 获取最近一次发生的错误编号
	Int32 GetSocketError();													// 获得最近一次发生的套接字错误
	std::string GetErrorString(Int32 nErrorID);								// 返回指定错误的描述

	// 网卡、路由相关函数
	std::string GetRouteIP(const std::string &strDestinationIP);			// 通过目标地址获取本地 IP
	Int32 GetMTUWithDestinationIP(const std::string &strDestinationIP);		// 通过目标地址获取网口的 MTU 值
	Int32 GetMTUWithInterface(const std::string &strInterfaceName);			// 通过网卡接口名称获取 MTU 值
	Int32 GetMTUWithLocalIP(const std::string &strLocalIP);					// 通过本地网卡 IP 地址获取 MTU 值
	Bool  GetMACAddress(std::list<std::string> &macAddressList);			// 获取本机的 MAC 地址列表

	std::string GetModuleFileName();										// 获得当前可执行模块的完整路径
	std::string GetModuleFilePath();										// 获得当前可执行模块所在的路径

	// 文件与目录操作函数
	Bool NormalizeFileName(std::string &strFileName);						// 将文件/文件夹名称标准化
	Bool IsFileExist(LPCSTR lpszFilename);									// 判断文件是否存在
	Bool MakeDirectory(LPCSTR lpszDirname);									// 创建文件夹，不支持递归创建
	Bool MakeDirectoryRecursively(LPCSTR lpszDirname);						// 创建文件夹，支持递归创建
	Bool RemoveDirectory(LPCSTR lpszDirname);								// 删除文件夹，文件夹必须为空
	Bool RemoveDirectoryRecursively(LPCSTR lpszDirname);					// 递归删除文件夹，文件夹允许非空
	Bool RemoveFile(LPCSTR lpszFilename);									// 删除指定的文件
    std::string FileGetContents(LPCSTR lpszFile);                           // 获取文件内容

	pid_t GetThreadID();

	// 时间相关函数
	std::string Localtime(Int64 lMillisec = 0);								// Format: yyyy-mm-dd hh:mm:ss
	std::string Localtime2(Int64 lMillisec = 0);							// Format: yyyy-mm-dd hh:mm:ss.MMM
	std::string Localtime3(Int64 lMillisec = 0);							// Format: Fri, 25 Mar 2016 13:23:35 CST
    std::string Localtime4(Int64 lMillisec = 0);							// Format: yyyy-mm-ddThh:mm:ssZ
	std::string GreenwichMeanTime(Int64 lMillisec = 0);						// Format: yyyy-mm-dd hh:mm:ss
	std::string GreenwichMeanTime2(Int64 lMillisec = 0);					// Format: yyyy-mm-dd hh:mm:ss.MMM
	std::string GreenwichMeanTime3(Int64 lMillisec = 0);					// Format: Fri, 25 Mar 2016 13:23:35 GMT
    std::string GreenwichMeanTime4(Int64 lMillisec = 0);					// Format: yyyy-mm-ddThh:mm:ssZ

    // UUID 生成器
    std::string CreateUUID();                                               // Format: a397a255-53be-41fc-996b-521413e9e22d
    std::string CreateUUID2();                                              // Format: a397a25553be41fc996b521413e9e22d
}

#endif
