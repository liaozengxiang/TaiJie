#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "../Include/Assert.h"

#ifndef NDEBUG

void Assert::Output(const char *__assertion, const char *__file, unsigned int __line)
{
	FILE *pFile = fopen("Assert.log", "a");
	if (pFile)
	{
		char szTime[32];
		time_t sysTime = time(NULL);
		tm *pTime = localtime(&sysTime);
		sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday,
															pTime->tm_hour, pTime->tm_min, pTime->tm_sec);

		fprintf(pFile, "Time:      %s\n", szTime);
		fprintf(pFile, "Assertion: %s\n", __assertion);
		fprintf(pFile, "File:      %s\n", __file);
		fprintf(pFile, "Line:      %d\n\n", __line);
		fclose(pFile);

		abort();
	}
}

#endif
