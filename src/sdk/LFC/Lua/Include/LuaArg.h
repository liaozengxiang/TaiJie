#ifndef ___LUA__ARG__20150813___
#define ___LUA__ARG__20150813___

#include <Common/Common.h>
#include <lua.hpp>
#include <lualib.h>
#include <string>
#include <map>
#include <set>

class CLuaArg
{
public:
	CLuaArg(lua_State *L, LPCSTR lpszFunc);
	virtual ~CLuaArg();

public:
	Int32 GetInputArgSize() const { return m_nInputArgSize; }

	CLuaArg& operator <<(LPCSTR str);
	CLuaArg& operator <<(const std::string &str);
	CLuaArg& operator <<(Double fValue);
	CLuaArg& operator <<(Int32 nValue);
	CLuaArg& operator <<(void *ptr);

	CLuaArg& operator >>(std::string &str);
	CLuaArg& operator >>(Double &fValue);
	CLuaArg& operator >>(Int32 &nValue);
	CLuaArg& operator >>(void *&ptr);
	CLuaArg& operator >>(std::map<std::string, std::string> &mapValues);
	CLuaArg& operator >>(std::set<std::string> &setValues);
	CLuaArg& operator >>(std::list<std::string> &lstValues);
	CLuaArg& operator >>(std::list<std::map<std::string, std::string> > &lstItems);

protected:
	lua_State*						m_pLua;
	Int32							m_nInputArgSize;
};

#endif
