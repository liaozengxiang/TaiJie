#ifndef ___MACRO__H__20150329___
#define ___MACRO__H__20150329___

#include "OSType.h"
#include <sched.h>

// BEGIN: likely/unlikely
#ifndef unlikely
	#define unlikely(x)      __builtin_expect(!!(x), 0)
	#define likely(x)        __builtin_expect(!!(x), 1)
#endif
// END: likely/unlikely

// BEGIN: atomic lock
#define AtomicInit(ptr) \
	do \
	{ \
		*ptr = 0; \
	} while (0)

#define AtomicTryLock(ptr) (__sync_bool_compare_and_swap(ptr, 0, 1))

#define AtomicLock(ptr) \
	do \
	{ \
		while (!__sync_bool_compare_and_swap(ptr, 0, 1)) \
		{ \
			sched_yield(); \
		} \
	} while (0)

#define AtomicUnlock(ptr) \
	do \
	{ \
		*ptr = 0; \
	} while (0)
// END: atomic lock

// BEGIN: 路径的最大长度
#ifndef MAX_PATH
	#define MAX_PATH 260
#endif
// END: 路径的最大长度

// BEGIN: max integer
#ifndef INT8_MAX
	#define INT8_MAX 0x7F
#endif

#ifndef UINT8_MAX
	#define UINT8_MAX 0xFF
#endif

#ifndef INT16_MAX
	#define INT16_MAX 0x7FFF
#endif

#ifndef UINT16_MAX
	#define UINT16_MAX 0xFFFF
#endif

#ifndef INT32_MAX
	#define INT32_MAX 0x7FFFFFFF
#endif

#ifndef UINT32_MAX
	#define UINT32_MAX 0xFFFFFFFF
#endif

#ifdef OS_64
	#ifndef INT64_MAX
		#define INT64_MAX 0x7FFFFFFFFFFFFFFFL
	#endif

	#ifndef UINT64_MAX
		#define UINT64_MAX 0xFFFFFFFFFFFFFFFFUL
	#endif
#else
	#ifndef INT64_MAX
		#define INT64_MAX 0x7FFFFFFFFFFFFFFFLL
	#endif

	#ifndef UINT64_MAX
		#define UINT64_MAX 0xFFFFFFFFFFFFFFFFULL
	#endif
#endif
// END: max integer

#endif
