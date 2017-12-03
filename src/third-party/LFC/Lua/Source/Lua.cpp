#include "../Include/Lua.h"

CLua::CLua()
{
	m_pLua = luaL_newstate();
	luaL_openlibs(m_pLua);
}

CLua::~CLua()
{
	if (m_pLua != NULL)
	{
		lua_close(m_pLua);
		m_pLua = NULL;
	}
}

Bool CLua::LoadScript(LPCSTR lpszLuaScript)
{
	return (luaL_dofile(m_pLua, lpszLuaScript) == 0);
}

Bool CLua::Call(CLuaArg *pArg, Int32 nOutputArgSize)
{
	Int32 nResult = lua_pcall(m_pLua, pArg->GetInputArgSize(), nOutputArgSize, 0);
	return (nResult == 0);
}

void CLua::RegisterFunction(LPCSTR lpszFunc, lua_CFunction func)
{
	lua_register(m_pLua, lpszFunc, func);
}
