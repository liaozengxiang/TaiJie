/*
 * 文件: OSType.h
 * 功能: 定义操作系统的基本数据类型
 * 作者: 廖增祥
 * 日期: 2013-04-28
 *
 * 备注: 几个重要的关于操作系统的宏
 *       OS_WINDOWS		- 代表此系统为 WINDOWS 系统
 *       OS_LINUX		- 代表此系统为 LINUX 系统
 *       OS_32			- 代表此系统为 32 位
 *       OS_64			- 代表此系统为 64 位
 */

#ifndef ___OSTYPE__H__20130428___
#define ___OSTYPE__H__20130428___

typedef float				Float;
typedef double				Double;
typedef void				Void;
typedef int					Bool;

#ifndef True
#define True				1
#endif

#ifndef TRUE
#define TRUE				1
#endif

#ifndef False
#define False				0
#endif

#ifndef FALSE
#define FALSE				0
#endif

#ifndef NULL
#define NULL				0
#endif
/*
#ifndef Null
#define Null				0
#endif
*/
#ifdef WIN32				// For Win32

// 定义 Windows 操作系统宏
#ifndef OS_WINDOWS
#define OS_WINDOWS
#endif

// 定义 32 位系统宏
#ifndef OS_32
#define OS_32
#endif

typedef char *				LPSTR;
typedef const char *		LPCSTR;
typedef char				Char;
typedef char				Int8;
typedef short				Int16;
typedef int					Int32;
typedef __int64				Int64;

typedef unsigned char		UInt8;
typedef unsigned short		UInt16;
typedef unsigned int		UInt32;
typedef unsigned __int64	UInt64;

#else						// For g++ 64-bit/32-bit

// 定义 Linux 操作系统宏
#ifndef OS_LINUX
#define OS_LINUX
#endif

typedef char *				LPSTR;
typedef const char *		LPCSTR;
typedef char				Char;
typedef char				Int8;
typedef short				Int16;
typedef int					Int32;
typedef unsigned char		UInt8;
typedef unsigned short		UInt16;
typedef unsigned int		UInt32;

#ifdef __i386__

// 定义 32 位系统宏
#ifndef OS_32
#define OS_32
#endif

typedef long long			Int64;
typedef unsigned long long	UInt64;
#endif

#ifdef __x86_64__

// 定义 64 位系统宏
#ifndef OS_64
#define OS_64
#endif

typedef long				Int64;
typedef unsigned long		UInt64;
#endif

#ifndef __x86_64__
#ifdef __amd64__

// 定义 64 位系统宏
#ifndef OS_64
#define OS_64
#endif

typedef long				Int64;
typedef unsigned long		UInt64;
#else

// 定义 32 位系统宏
#ifndef OS_32
#define OS_32
#endif

typedef long long			Int64;
typedef unsigned long long	UInt64;
#endif
#endif

#endif

#ifdef OS_WINDOWS
#ifndef INC_WINDOWS_H
#define INC_WINDOWS_H
#include <winsock2.h>
#endif
#endif

// DLL export definition
#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllimport))
    #else
      #define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define DLL_PUBLIC __attribute__ ((visibility ("default")))
    #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_PUBLIC
    #define DLL_LOCAL
  #endif
#endif
// DLL export definition end

#endif  // ___OSTYPE__H__20130428___
