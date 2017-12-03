#include "../Include/LuaArg.h"

CLuaArg::CLuaArg(lua_State *L, LPCSTR lpszFunc)
{
	Assert(L != NULL);
	m_pLua = L;
	m_nInputArgSize = 0;

	lua_getglobal(m_pLua, lpszFunc);
}

CLuaArg::~CLuaArg()
{
}

/************************************************************************/
/* 插入运算符重载                                                       */
/************************************************************************/
CLuaArg& CLuaArg::operator <<(LPCSTR str)
{
	lua_pushstring(m_pLua, str);
	++m_nInputArgSize;

	return *this;
}

CLuaArg& CLuaArg::operator <<(const std::string &str)
{
	lua_pushlstring(m_pLua, str.c_str(), str.length());
	++m_nInputArgSize;

	return *this;
}

CLuaArg& CLuaArg::operator <<(Double fValue)
{
	lua_pushnumber(m_pLua, fValue);
	++m_nInputArgSize;

	return *this;
}

CLuaArg& CLuaArg::operator <<(Int32 nValue)
{
	lua_pushinteger(m_pLua, nValue);
	++m_nInputArgSize;

	return *this;
}

CLuaArg& CLuaArg::operator <<(void *ptr)
{
	lua_pushlightuserdata(m_pLua, ptr);
	++m_nInputArgSize;

	return *this;
}

/************************************************************************/
/* 提取运算符重载                                                       */
/************************************************************************/
CLuaArg& CLuaArg::operator >>(std::string &str)
{
	size_t nLen = 0;
	LPCSTR lpszStr = lua_tolstring(m_pLua, -1, &nLen);
	str.assign(lpszStr, nLen);
	lua_pop(m_pLua, 1);

	return *this;
}

CLuaArg& CLuaArg::operator >>(Double &fValue)
{
	fValue = lua_tonumber(m_pLua, -1);
	lua_pop(m_pLua, 1);

	return *this;
}

CLuaArg& CLuaArg::operator >>(Int32 &nValue)
{
	nValue = lua_tointeger(m_pLua, -1);
	lua_pop(m_pLua, 1);

	return *this;
}

CLuaArg& CLuaArg::operator >>(void *&ptr)
{
	ptr = lua_touserdata(m_pLua, -1);
	lua_pop(m_pLua, 1);

	return *this;
}

CLuaArg& CLuaArg::operator >>(std::map<std::string, std::string> &mapValues)
{
	Int32 nIndex = lua_gettop(m_pLua);
	lua_pushnil(m_pLua);

	while (lua_next(m_pLua, nIndex))
	{
		mapValues[lua_tostring(m_pLua, -2)] = lua_tostring(m_pLua, -1);
		lua_pop(m_pLua, 1);
	}
	
	lua_pop(m_pLua, 1);
	return *this;
}

CLuaArg& CLuaArg::operator >>(std::set<std::string> &setValues)
{
	Int32 nIndex = lua_gettop(m_pLua);
	lua_pushnil(m_pLua);

	while (lua_next(m_pLua, nIndex))
	{
		setValues.insert(lua_tostring(m_pLua, -1));
		lua_pop(m_pLua, 1);
	}

	lua_pop(m_pLua, 1);
	return *this;
}

CLuaArg& CLuaArg::operator >>(std::list<std::string> &lstValues)
{
	Int32 nIndex = lua_gettop(m_pLua);
	lua_pushnil(m_pLua);

	while (lua_next(m_pLua, nIndex))
	{
		lstValues.push_back(lua_tostring(m_pLua, -1));
		lua_pop(m_pLua, 1);
	}

	lua_pop(m_pLua, 1);
	return *this;
}

CLuaArg& CLuaArg::operator >>(std::list<std::map<std::string, std::string> > &lstItems)
{
	Int32 nIndex = lua_gettop(m_pLua);
	lua_pushnil(m_pLua);

	while (lua_next(m_pLua, nIndex))
	{
		Int32 nIndex2 = lua_gettop(m_pLua);
		lua_pushnil(m_pLua);

		std::map<std::string, std::string> mapNull;
		lstItems.push_back(mapNull);
		std::map<std::string, std::string> &mapValues = lstItems.back();

		while (lua_next(m_pLua, nIndex2))
		{
			mapValues[lua_tostring(m_pLua, -2)] = lua_tostring(m_pLua, -1);
			lua_pop(m_pLua, 1);
		}

		lua_pop(m_pLua, 1);
	}

	lua_pop(m_pLua, 1);
	return *this;
}
