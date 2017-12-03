#ifndef ___LUA__20150813___
#define ___LUA__20150813___

#include <Common/Common.h>
#include <lua.hpp>
#include <lualib.h>
#include "LuaArg.h"

class CLua
{
public:
	CLua();
	virtual ~CLua();

public:
	lua_State* GetLuaState() const { return m_pLua; }
	Bool LoadScript(LPCSTR lpszLuaScript);
	Bool Call(CLuaArg *pArg, Int32 nOutputArgSize);
	void RegisterFunction(LPCSTR lpszFunc, lua_CFunction func);

protected:
	lua_State*						m_pLua;
};

#endif
