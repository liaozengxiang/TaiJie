#ifndef ___ASSERT__H__20140418___
#define ___ASSERT__H__20140418___

// Debug 模式下对 Assert 宏的定义，如果是 Release 模式，需要预定义 NDEBUG 宏
#ifndef NDEBUG
	#include <assert.h>
	
	namespace Assert
	{
		void Output(const char *__assertion, const char *__file, unsigned int __line);
	}

	#define Assert(expr) ((expr)										\
				? (void)0												\
				: Assert::Output(#expr, __FILE__, __LINE__))
#else
	#define Assert(expr) (void)0
#endif

#endif
