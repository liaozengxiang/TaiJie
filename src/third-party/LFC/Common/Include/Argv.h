/*
 * Argv.h
 *
 *  Created on: Oct 19, 2013
 *      Author: lory
 */

#ifndef ARGV_H_
#define ARGV_H_

#include <map>
#include <string>
#include "OSType.h"

class CArgv
{
	typedef struct tagARGUMENT_ITEM
	{
		std::string				strDescription;		// 参数的描述
		Bool					bValueType;			// 该参数是值型还是启用型
		std::string				strItemValue;		// 当 bValueType=True 时有效
		Bool					bItemEnabled;		// 当 bValueType=False 时有效
	} ARGUMENT_ITEM, *LPARGUMENT_ITEM;

public:
	CArgv();
	virtual ~CArgv();

public:
	void SetProject(LPCSTR lpszProject, LPCSTR lpszVersion, LPCSTR lpszBuild);
	void SetBrief(LPCSTR lpszBrief);
	void AddParameterValue(LPCSTR lpszOption, LPCSTR lpszDesc, LPCSTR lpszDefaultValue);
	void AddParameterEnable(LPCSTR lpszOption, LPCSTR lpszDesc, Bool bDefaultValue);
	Bool AcceptParameters(Int32 argc, LPSTR argv[]);
	std::string GetParameterValue(LPCSTR lpszOption);
	Bool IsParameterEnable(LPCSTR lpszOption);
	void PrintUsage();

private:
	std::string								m_strProject;
	std::string								m_strVersion;
	std::string								m_strBrief;
	std::map<std::string, ARGUMENT_ITEM>	m_mapArguments;
};

#endif
