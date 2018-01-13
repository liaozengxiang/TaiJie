#include "../Include/OS.h"
#include "../Include/SocketUtil.h"
#include "../Include/Macro.h"
#include "../Include/STR.h"
#include <sys/syscall.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sstream>
#include <list>
#include <string>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <dirent.h>

#ifdef OS_LINUX
	#include <errno.h>
	#include <unistd.h>
	#include <signal.h>
	#include <fcntl.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <sys/time.h>
	#include <sys/ioctl.h>
	#include <net/if.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#else
	#include <time.h>
	#pragma comment(lib, "WS2_32.lib")
#endif

// 静态函数声明
static Bool LoadRouteTable();

Bool OS::Initialize()
{
	Bool bSuccess = False;

	do
	{
        // 初始化随机种子
        srand(time(NULL));

		if (!SocketUtil::Initialize())
		{
			break;
		}

		if (!LoadRouteTable())
		{
			break;
		}

		bSuccess = True;
	} while (0);

	return bSuccess;
}

Bool OS::InitDaemon()
{
#ifdef OS_LINUX
/*
	pid_t pid;
	Int32 nNullFD;
	
	// Exit parent process, child process continued
	pid = fork();
	if (pid < 0)
	{
		// Create process failed
		return False;
	}
	
	if (pid > 0)
	{
		// Exit the parent process
		exit(0);
	}
	
	// Come to child process
	// Create new session and become the leader
	setsid();
	
	// redirect stdin, stdout, stderr to /dev/null
	nNullFD = open("/dev/null", O_RDWR);
	if (nNullFD != -1)
	{
		dup2(nNullFD, 0);
		dup2(nNullFD, 1);
		dup2(nNullFD, 2);
		close(nNullFD);
	}
	
	// Clear file mode creation mask
	umask(022);
*/
	// 将进程设置为后台进程，使其父进程为 1
	if (daemon(1, 0) == -1)
	{
		return False;
	}

	// 忽略特定的信号
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);

	return True;
#else
	return True;
#endif
}

void OS::RegisterSignal(Int32 nSignal, void (*SigFunc)(Int32 nSignal))
{
	signal(nSignal, SigFunc);
}

void OS::SetProcessTitle(LPCSTR lpszTitle, Int32 argc, char *argv[])
{
	// 计算 argv 指向的连续内存长度
	Int32 nArgvSize = 0;
	for (Int32 i = 0; argv[i] != NULL; ++i)
	{
		nArgvSize += strlen(argv[i]) + 1;
	}

	// 如果 argv 的内存与 environ 的内存不连续，则表示 environ 内存已经被复制过一次
	if (argv[0] + nArgvSize != environ[0])
	{
		// 将进程标题设置到 argv[0] 指示的内存中，并将 argv[1] 设置为 NULL
		argv[1] = NULL;
		strcpy(argv[0], lpszTitle);
	}
	else
	{
		// 计算 environ 指向的连续内存长度
		Int32 nEnvironSize = 0;
		Int32 i;
		for (i = 0; environ[i] != NULL; ++i)
		{
			nEnvironSize += strlen(environ[i]) + 1;
		}

		// 将 environ 参数复制到新的内存中
		char *pNewEnviron = (char *)malloc(nEnvironSize);
		for (i = 0; environ[i] != NULL; ++i)
		{
			strcpy(pNewEnviron, environ[i]);
			environ[i] = pNewEnviron;
			pNewEnviron += strlen(pNewEnviron) + 1;
		}

		// 将进程标题设置到 argv[0] 指示的内存中，并将 argv[1] 设置为 NULL
		argv[1] = NULL;
		strncpy(argv[0], lpszTitle, nArgvSize + nEnvironSize);
	}
}

void OS::Sleep(Int32 nMillisecond)
{
#ifdef OS_WINDOWS
	::Sleep(nMillisecond);
#else
	usleep(nMillisecond * 1000L);
#endif
}

Int32 OS::GetCoreCount()
{
	return sysconf(_SC_NPROCESSORS_CONF);
}

Bool OS::SetThreadAffinity(Int32 nCPUIndex, Int32 nThreadID)
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(nCPUIndex, &mask);
	return (sched_setaffinity(nThreadID, sizeof(mask), &mask) == 0);
}

Int64 OS::Milliseconds()
{
#ifdef OS_LINUX
	struct timeval tv;
	if (-1 == gettimeofday(&tv, NULL))
	{
		return 0;
	}
	
	return (tv.tv_sec * (Int64)1000 + tv.tv_usec / 1000);
#else
	SYSTEMTIME sysTime;
	FILETIME fileTime;
	LARGE_INTEGER li;
	
	GetLocalTime(&sysTime);
	SystemTimeToFileTime(&sysTime, &fileTime);
	
	li.LowPart = fileTime.dwLowDateTime;
	li.HighPart = fileTime.dwHighDateTime;

	return li.QuadPart / 10000;
#endif
}

Int64 OS::GetTickCount()
{
#ifdef OS_LINUX
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#else
    return ::GetTickCount();
#endif
}

Bool OS::SetResourceLimit(Int32 nResource, Int64 lCur, Int64 lMax)
{
    struct rlimit rl;
    rl.rlim_cur = lCur;
    rl.rlim_max = lMax;

    return (setrlimit(nResource, &rl) == 0);
}

Bool OS::SetMaxFDSize(Int64 lSize)
{
    return SetResourceLimit(RLIMIT_NOFILE, lSize, lSize);
}

Bool OS::SetCoreDumpSize(Int64 lSize)
{
    return SetResourceLimit(RLIMIT_CORE, lSize, lSize);
}

Int32 OS::GetLastError()
{
#ifdef OS_WINDOWS
	return ::GetLastError();
#else
	return errno;
#endif
}

Int32 OS::GetSocketError()
{
#ifdef OS_WINDOWS
	return WSAGetLastError();
#else
	return errno;
#endif
}

std::string OS::GetErrorString(Int32 nErrorID)
{
	char szError[64];
	strerror_r(nErrorID, szError, 64);
    
    return szError;
}


/************************************************************************/
/* 路由表相关函数                                                       */
/************************************************************************/

struct IP_TABLE_ITEM
{
	std::string	strInterfaceName;					// 网络接口名称，如：eth0
	std::string	strLocalIP;							// 网络接口配置的本地 IP 地址，如: 172.20.100.131
	UInt32		dwDestinationIP;					// 目标 IP（子网掩码有效的位），如: 172.20.100.0
	UInt32		dwMask;								// 子网掩码，如: 255.255.255.0
};

static std::list<IP_TABLE_ITEM> s_lstIPTables;

/*
 * 功能: 装载路由表
 * 返回: 成功返回 TRUE; 否则返回 FALSE
 * 算法: 读取 /proc/net/route 文件，取出接口名称、目的地址、子网掩码，再次根据接口名称取出本地对应的 IP 地址
 */
static Bool LoadRouteTable()
{
	/*
	[root@vasms src]# cat /proc/net/route
	Iface	Destination	Gateway 	Flags	RefCnt	Use	Metric	Mask		MTU	Window	IRTT
	eth2	0068A8C0	00000000	0001	0		0	0		00FFFFFF	0	0		0
	eth2	0000FEA9	00000000	0001	0		0	0		0000FFFF	0	0		0
	eth0	000014AC	00000000	0001	0		0	0		0000FFFF	0	0		0
	eth0	00000000	FBFE14AC	0003	0		0	0		00000000	0	0		0
	*/
	FILE *pFile = fopen("/proc/net/route", "r");
	if (NULL == pFile)
	{
		return FALSE;
	}

	char szLine[1024];
	char szInterfaceName[32];
	UInt32 dwDestinationIP;
	UInt32 dwIgnore1, dwIgnore2;
	Int32 nIgnore3, nIgnore4, nIgnore5;
	UInt32 dwMask;

	while (1)
	{
		szLine[1023] = '\0';
		if (fgets(szLine, 1023, pFile) == NULL)
		{
			break;
		}

		if (sscanf(szLine, "%s%x%x%d%d%d%d%x", szInterfaceName, &dwDestinationIP, &dwIgnore1, &dwIgnore2, &nIgnore3, &nIgnore4, &nIgnore5, &dwMask) != 8)
		{
			continue;
		}

		// 获取网络接口对应的 IP 地址
		UInt32 dwLocalIP = 0;
		Int32 sockfd = -1;
		struct ifreq ifr;
		struct sockaddr_in *addr = NULL;

		memset(&ifr, 0, sizeof(struct ifreq));
		strcpy(ifr.ifr_name, szInterfaceName);
		addr = (struct sockaddr_in *)&ifr.ifr_addr;
		addr->sin_family = AF_INET;

		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			continue;
		}

		if (ioctl(sockfd, SIOCGIFADDR, &ifr) == 0)
		{
			dwLocalIP = addr->sin_addr.s_addr;
		}

		close(sockfd);
		if (dwLocalIP == 0)
		{
			continue;
		}

		char szLocalIP[32];
		sprintf(szLocalIP, "%d.%d.%d.%d", (dwLocalIP&0xFF), ((dwLocalIP>>8)&0xFF), ((dwLocalIP>>16)&0xFF), ((dwLocalIP>>24)&0xFF));

		IP_TABLE_ITEM item;
		item.strInterfaceName = szInterfaceName;
		item.strLocalIP       = szLocalIP;
		item.dwDestinationIP  = dwDestinationIP;
		item.dwMask           = dwMask;
		s_lstIPTables.push_back(item);
	}

	fclose(pFile);
	return True;
}

/*
 * 功能: 根据目标地址返回本地 IP 地址
 * 返回: 成功返回路由到目标 IP 地址的本地 IP 地址；失败返回 255.255.255.255
 */
std::string OS::GetRouteIP(const std::string &strDestinationIP)
{
	UInt32 dwDestinationIP = inet_addr(strDestinationIP.c_str());
	for (std::list<IP_TABLE_ITEM>::const_iterator it = s_lstIPTables.begin(); it != s_lstIPTables.end(); ++it)
	{
		if ((dwDestinationIP & it->dwMask) == it->dwDestinationIP)
		{
			return it->strLocalIP;
		}
	}

	return "255.255.255.255";
}

Int32 OS::GetMTUWithDestinationIP(const std::string &strDestinationIP)
{
	UInt32 dwDestinationIP = inet_addr(strDestinationIP.c_str());
	for (std::list<IP_TABLE_ITEM>::const_iterator it = s_lstIPTables.begin(); it != s_lstIPTables.end(); ++it)
	{
		if ((dwDestinationIP & it->dwMask) == it->dwDestinationIP)
		{
			return GetMTUWithInterface(it->strInterfaceName);
		}
	}

	return 1500;
}

Int32 OS::GetMTUWithInterface(const std::string &strInterfaceName)
{
	char szMTUFile[64];
	sprintf(szMTUFile, "/sys/class/net/%s/mtu", strInterfaceName.c_str());
	FILE *pFile = fopen(szMTUFile, "r");
	if (NULL == pFile)
	{
		return 1500;
	}

	Int32 nMTU = 1500;
	fscanf(pFile, "%d", &nMTU);
	fclose(pFile);

	return nMTU;
}

Int32 OS::GetMTUWithLocalIP(const std::string &strLocalIP)
{
	for (std::list<IP_TABLE_ITEM>::const_iterator it = s_lstIPTables.begin(); it != s_lstIPTables.end(); ++it)
	{
		if (it->strLocalIP == strLocalIP)
		{
			return GetMTUWithInterface(it->strInterfaceName);
		}
	}

	return 1500;
}

Bool OS::GetMACAddress(std::list<std::string> &macAddressList)
{
    if (s_lstIPTables.empty())
    {
        return False;
    }

    for (std::list<IP_TABLE_ITEM>::const_iterator it = s_lstIPTables.begin(); it != s_lstIPTables.end(); ++it)
    {
        char szAddressFile[MAX_PATH];
        sprintf(szAddressFile, "/sys/class/net/%s/address", it->strInterfaceName.c_str());

        // 读取文件中的内容
        std::string strContents = FileGetContents(szAddressFile);

        // 去掉行尾，只取前 17 字节内容
        char szMac[24];
        memcpy(szMac, strContents.c_str(), 17);
        szMac[17] = '\0';
        
        macAddressList.push_back(szMac);
    }

	return True;
}

std::string OS::GetModuleFileName()
{
#ifdef OS_WINDOWS
	Char szFileName[MAX_PATH];
	::GetModuleFileName(NULL, szFileName, MAX_PATH);
	return szFileName;
#else
	Char szFileName[260];
	Int32 nPathSize = readlink("/proc/self/exe", szFileName, 260);
	if (nPathSize == -1)
	{
		return "";
	}

	szFileName[nPathSize] = '\0';
	return szFileName;
#endif
}

std::string OS::GetModuleFilePath()
{
#ifdef OS_WINDOWS
	Char szFileName[MAX_PATH];
	::GetModuleFileName(NULL, szFileName, MAX_PATH);
	Char c = '\\';
#else
	Char szFileName[260];
	Int32 nPathSize = readlink("/proc/self/exe", szFileName, 260);
	if (nPathSize == -1)
	{
		return "";
	}
	
	szFileName[nPathSize] = '\0';
	Char c = '/';
#endif

	// 从最后找到第一个目录分隔符
	Int32 nIndex = strlen(szFileName) - 1;
	while (szFileName[nIndex] != c && nIndex >= 0)
	{
		--nIndex;
	}

	szFileName[nIndex + 1] = '\0';
	return szFileName;
}

Bool OS::NormalizeFileName(std::string &strFileName)
{
    Bool bSuccess = True;

    // 将路径按 / 进行分割
    std::string strFileName1 = strFileName;
    if (strFileName.at(0) != '/')
    {
        strFileName1 = GetModuleFilePath() + strFileName;
    }

    std::list<std::string> lstItems;
    STR::Explode(strFileName1.c_str(), "/", lstItems);

    std::list<std::string> lstNormalizedItems;
    std::list<std::string>::iterator it;
    for (it = lstItems.begin(); it != lstItems.end(); ++it)
    {
        if (it->empty() || *it == ".")
        {
            continue;
        }
        else if (*it == "..")
        {
            if (lstNormalizedItems.empty())
            {
                bSuccess = False;
                break;
            }

            lstNormalizedItems.pop_back();
        }
        else
        {
            lstNormalizedItems.push_back(*it);
        }
    }

    if (!bSuccess)
    {
        return False;
    }

    // 将已标准化的分段合并
    strFileName.clear();
    for (it = lstNormalizedItems.begin(); it != lstNormalizedItems.end(); ++it)
    {
        strFileName += "/";
        strFileName += *it;
    }

    // 对结尾进行处理，如果是文件夹，则增加 "/"
    // 如果文件不可访问，则同样返回成功
    struct stat st;
    if (lstat(strFileName.c_str(), &st) == 0)
    {
        if (S_ISDIR(st.st_mode))
        {
            strFileName.push_back('/');
        }
    }

    return bSuccess;
}

Bool OS::IsFileExist(LPCSTR lpszFilename)
{
#ifdef OS_LINUX
	if (0 == access(lpszFilename, F_OK))
	{
		return True;
	}
	return False;
#endif
}

Bool OS::MakeDirectory(LPCSTR lpszDirname)
{
#ifdef OS_LINUX
	if (0 != mkdir(lpszDirname, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH))
	{
		if (GetLastError() != EEXIST)
		{
			return False;
		}
	}
#else
	if (!CreateDirectoryA(lpszDirname, NULL))
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
			return False;
		}
	}
#endif
	return True;
}

Bool OS::MakeDirectoryRecursively(LPCSTR lpszDirname)
{
	Bool bFullPath = False;
	if (lpszDirname[0] == '/')
	{
		bFullPath = True;
	}

	std::string strDirname(lpszDirname);
	
	std::list<std::string> lstDir;
	STR::Explode(strDirname.c_str(), "/", lstDir);
	std::list<std::string>::iterator it = lstDir.begin();
	std::ostringstream oss;
	if (bFullPath)
	{
		oss << "/";
	}

	for (; it != lstDir.end(); ++it)
	{
		if (it->empty())
		{
			continue;
		}

		oss << *it << "/";
		if (!IsFileExist(oss.str().c_str()))
		{
			if (!MakeDirectory(oss.str().c_str()))
			{
				return False;
			}
		}
	}

	return True;
}

Bool OS::RemoveDirectory(LPCSTR lpszDirname)
{
	return (remove(lpszDirname) == 0);
}

Bool OS::RemoveDirectoryRecursively(LPCSTR lpszDirname)
{
    std::string strDirname = lpszDirname;
    NormalizeFileName(strDirname);

    DIR *pDIR = opendir(strDirname.c_str());
    if (NULL == pDIR)
    {
        return False;
    }

    struct dirent entry;
    struct dirent *pResult;
    while (readdir_r(pDIR, &entry, &pResult) == 0)
    {
        if (NULL == pResult)
        {
            break;
        }

        if (0 == strcmp(entry.d_name, ".") || 0 == strcmp(entry.d_name, ".."))
        {
            continue;
        }

        Bool bIsDir = False;
        if (entry.d_type != DT_UNKNOWN)
        {
            bIsDir = (entry.d_type == DT_DIR);
        }
        else
        {
            char szFullPath[MAX_PATH];
            strcpy(szFullPath, strDirname.c_str());
            strcat(szFullPath, entry.d_name);

            struct stat st;
            if (stat(szFullPath, &st) == -1)
            {
                continue;
            }

            bIsDir = S_ISDIR(st.st_mode) ? True : False;
        }

        char szFileName[MAX_PATH];
        strcpy(szFileName, strDirname.c_str());
        strcat(szFileName, entry.d_name);
        if (!bIsDir)
        {
            remove(szFileName);
        }
        else
        {
            strcat(szFileName, "/");
            RemoveDirectoryRecursively(szFileName);
        }
    }

    closedir(pDIR);
    remove(lpszDirname);

    return True;
}

Bool OS::RemoveFile(LPCSTR lpszFilename)
{
	return (remove(lpszFilename) == 0);
}

std::string OS::FileGetContents(LPCSTR lpszFile)
{
    std::string strContent;
    FILE *pFile = NULL;
    char buf[4096];
    
    do
    {
        pFile = fopen(lpszFile, "rb");
        if (NULL == pFile)
        {
            break;
        }

        fseek(pFile, 0, SEEK_END);
        Int64 lFileSize = ftell(pFile);
        rewind(pFile);

        if (lFileSize > 0)
        {
            strContent.reserve(lFileSize + 1);
        }

        while (1)
        {
            Int32 nReadSize = fread(buf, 1, 4096, pFile);
            if (nReadSize == 0)
            {
                break;
            }

            strContent.append(buf, nReadSize);
        }
    } while (0);

    if (NULL != pFile)
    {
        fclose(pFile);
    }

    return strContent;
}

pid_t OS::GetThreadID()
{
	return syscall(SYS_gettid);
}

std::string OS::Localtime(Int64 lMillisec)
{
	if (lMillisec == 0)
	{
		lMillisec = Milliseconds();
	}

	time_t v = lMillisec / 1000;
	struct tm t;
	localtime_r(&v, &t);

	// yyyy-mm-dd hh:mm:ss
	char szTime[32];
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", 1900 + t.tm_year, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	return szTime;
}

std::string OS::Localtime2(Int64 lMillisec)
{
	if (lMillisec == 0)
	{
		lMillisec = Milliseconds();
	}

	time_t v = lMillisec / 1000;
	struct tm t;
	localtime_r(&v, &t);

	// yyyy-mm-dd hh:mm:ss.MMM
	char szTime[32];
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d.%03d", 1900 + t.tm_year, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, (Int32)(lMillisec % 1000));
	return szTime;
}

std::string OS::Localtime3(Int64 lMillisec)
{
	if (lMillisec == 0)
	{
		lMillisec = Milliseconds();
	}

	// 星期的字符串定义，从星期天至星期六
	static LPCSTR lpszWeek[] =
	{
		"Sun",
		"Mon",
		"Tue",
		"Wed",
		"Thu",
		"Fri",
		"Sat"
	};

	// 月份的字符串定义，从一月至十二月
	static LPCSTR lpszMonth[] =
	{
		"Jan",				// struct tm::tm_mon 的值 0 表示一月
		"Feb",
		"Mar",
		"Apr",
		"May",
		"Jun",
		"Jul",
		"Aug",
		"Sep",
		"Oct",
		"Nov",
		"Dec"
	};

	time_t v = lMillisec / 1000;
	struct tm t;
	localtime_r(&v, &t);

	// Fri, 25 Mar 2016 13:23:35 CST
	char szTime[64];
	sprintf(szTime, "%s, %02d %s %d %02d:%02d:%02d CST", lpszWeek[t.tm_wday], t.tm_mday, lpszMonth[t.tm_mon], t.tm_year + 1900, t.tm_hour, t.tm_min, t.tm_sec);
	return szTime;
}

std::string OS::Localtime4(Int64 lMillisec)
{
    if (lMillisec == 0)
    {
        lMillisec = Milliseconds();
    }

    time_t v = lMillisec / 1000;
    struct tm t;
    localtime_r(&v, &t);

    // yyyy-mm-dd hh:mm:ss
    char szTime[32];
    sprintf(szTime, "%04d-%02d-%02dT%02d:%02d:%02dZ", 1900 + t.tm_year, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
    return szTime;
}

std::string OS::GreenwichMeanTime(Int64 lMillisec)
{
	if (lMillisec == 0)
	{
		lMillisec = Milliseconds();
	}

	time_t v = lMillisec / 1000;
	struct tm t;
	gmtime_r(&v, &t);

	// yyyy-mm-dd hh:mm:ss
	char szTime[32];
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", 1900 + t.tm_year, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	return szTime;
}

std::string OS::GreenwichMeanTime2(Int64 lMillisec)
{
	if (lMillisec == 0)
	{
		lMillisec = Milliseconds();
	}

	time_t v = lMillisec / 1000;
	struct tm t;
	gmtime_r(&v, &t);

	// yyyy-mm-dd hh:mm:ss.MMM
	char szTime[32];
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d.%03d", 1900 + t.tm_year, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, (Int32)(lMillisec % 1000));
	return szTime;
}

std::string OS::GreenwichMeanTime3(Int64 lMillisec)
{
	if (lMillisec == 0)
	{
		lMillisec = Milliseconds();
	}

	// 星期的字符串定义，从星期天至星期六
	static LPCSTR lpszWeek[] =
	{
		"Sun",
		"Mon",
		"Tue",
		"Wed",
		"Thu",
		"Fri",
		"Sat"
	};

	// 月份的字符串定义，从一月至十二月
	static LPCSTR lpszMonth[] =
	{
		"Jan",				// struct tm::tm_mon 的值 0 表示一月
		"Feb",
		"Mar",
		"Apr",
		"May",
		"Jun",
		"Jul",
		"Aug",
		"Sep",
		"Oct",
		"Nov",
		"Dec"
	};

	time_t v = lMillisec / 1000;
	struct tm t;
	gmtime_r(&v, &t);

	// Fri, 25 Mar 2016 13:23:35 GMT
	char szTime[64];
	sprintf(szTime, "%s, %02d %s %d %02d:%02d:%02d GMT", lpszWeek[t.tm_wday], t.tm_mday, lpszMonth[t.tm_mon], t.tm_year + 1900, t.tm_hour, t.tm_min, t.tm_sec);
	return szTime;
}

std::string OS::GreenwichMeanTime4(Int64 lMillisec)
{
    if (lMillisec == 0)
    {
        lMillisec = Milliseconds();
    }

    time_t v = lMillisec / 1000;
    struct tm t;
    gmtime_r(&v, &t);

    // yyyy-mm-ddThh:mm:ssZ
    char szTime[32];
    sprintf(szTime, "%04d-%02d-%02dT%02d:%02d:%02dZ", 1900 + t.tm_year, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
    return szTime;
}

std::string OS::CreateUUID()
{
    char buf[37];
    const char *c = "89ab";
    char *p = buf;
    
    for (int n = 0; n < 16; ++n)
    {
        int b = rand() % 255;
        switch (n)
        {
        case 6:
            sprintf(p, "4%x", b % 15);
            break;

        case 8:
            sprintf(p, "%c%x", c[rand() % strlen(c)], b % 15);
            break;

        default:
            sprintf(p, "%02x", b);
            break;
        }

        p += 2;
        switch (n)
        {
        case 3:
        case 5:
        case 7:
        case 9:
            *p++ = '-';
            break;
        }
    }

    *p = '\0';
    return buf;
}

std::string OS::CreateUUID2()
{
    char buf[37];
    const char *c = "89ab";
    char *p = buf;

    for (int n = 0; n < 16; ++n)
    {
        int b = rand() % 255;
        switch (n)
        {
        case 6:
            sprintf(p, "4%x", b % 15);
            break;

        case 8:
            sprintf(p, "%c%x", c[rand() % strlen(c)], b % 15);
            break;

        default:
            sprintf(p, "%02x", b);
            break;
        }

        p += 2;
    }

    *p = '\0';
    return buf;
}
