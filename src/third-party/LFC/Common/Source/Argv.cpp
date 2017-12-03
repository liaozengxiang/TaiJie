/*
 * Argument.cpp
 *
 *  Created on: Oct 19, 2013
 *      Author: lory
 */

#include <stdio.h>
#include "../Include/Argv.h"

CArgv::CArgv()
{
	m_strBrief   = "This is a brief.";
	m_strProject = "Project";
	m_strVersion = "V1.0.0";
}

CArgv::~CArgv()
{
}

void CArgv::PrintUsage()
{
	Int32 nWidth = 0;
	std::map<std::string, ARGUMENT_ITEM>::iterator it;

	printf("%s_%s, build on %s %s\n", m_strProject.c_str(), m_strVersion.c_str(), __DATE__, __TIME__);
	printf("%s\n", m_strBrief.c_str());
	printf("Usage: %s", m_strProject.c_str());
	for (it = m_mapArguments.begin(); it != m_mapArguments.end(); ++ it)
	{
		printf(" [%s]", it->first.c_str());
	}
	printf("\n");

	printf("Options:\n");
	for (it = m_mapArguments.begin(); it != m_mapArguments.end(); ++ it)
	{
		if ((Int32)(it->first.length()) > nWidth)
		{
			nWidth = it->first.length();
		}
	}

	Int32 nSpaceWidth;
	for (it = m_mapArguments.begin(); it != m_mapArguments.end(); ++ it)
	{
		printf("  %s", it->first.c_str());

		nSpaceWidth = nWidth - it->first.length() + 4;
		while (nSpaceWidth --)
		{
			printf(" ");
		}

		printf("%s\n", it->second.strDescription.c_str());
	}
}

void CArgv::SetProject(LPCSTR lpszProject, LPCSTR lpszVersion, LPCSTR lpszBuild)
{
	m_strProject = lpszProject;
	m_strVersion = lpszVersion;
	m_strVersion += ".";
	m_strVersion += lpszBuild;
}

void CArgv::SetBrief(LPCSTR lpszBrief)
{
	m_strBrief = lpszBrief;
}

void CArgv::AddParameterValue(LPCSTR lpszOption, LPCSTR lpszDesc, LPCSTR lpszDefaultValue)
{
	ARGUMENT_ITEM argItem;
	argItem.bValueType    = True;
	argItem.strDescription = lpszDesc;
	argItem.strItemValue   = lpszDefaultValue;

	m_mapArguments[lpszOption] = argItem;
}

void CArgv::AddParameterEnable(LPCSTR lpszOption, LPCSTR lpszDesc, Bool bDefaultValue)
{
	ARGUMENT_ITEM argItem;
	argItem.bValueType  = False;
	argItem.strDescription = lpszDesc;
	argItem.bItemEnabled   = bDefaultValue;

	m_mapArguments[lpszOption] = argItem;
}

Bool CArgv::AcceptParameters(Int32 argc, Char *argv[])
{
	Bool bSuccess = True;
	std::map<std::string, ARGUMENT_ITEM>::iterator it;

	do
	{
		for (Int32 i = 1; i < argc; ++ i)
		{
			it = m_mapArguments.find(argv[i]);
			if (it == m_mapArguments.end())
			{
				bSuccess = False;
				printf("\033[1;1;31mERROR: Unsupported option `%s`\033[0m\n", argv[i]);
				break;
			}

			if (it->second.bValueType)
			{
				if (i + 1 == argc)
				{
					bSuccess = False;
					printf("\033[1;1;31mERROR: Unsupported option `%s`\033[0m\n", argv[i]);
					break;
				}

				it->second.strItemValue = argv[++ i];
			}
			else
			{
				it->second.bItemEnabled = True;
			}
		}

	} while (0);

	if (!bSuccess)
	{
		PrintUsage();
	}

	return bSuccess;
}

std::string CArgv::GetParameterValue(LPCSTR lpszOption)
{
	std::map<std::string, ARGUMENT_ITEM>::iterator it = m_mapArguments.find(lpszOption);
	if (it != m_mapArguments.end())
	{
		return it->second.strItemValue;
	}

	return "";
}

Bool CArgv::IsParameterEnable(LPCSTR lpszOption)
{
	std::map<std::string, ARGUMENT_ITEM>::iterator it = m_mapArguments.find(lpszOption);
	if (it != m_mapArguments.end())
	{
		return it->second.bItemEnabled;
	}

	return False;
}
