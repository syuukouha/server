/**
FileName : LuaNetModule.cpp
Author   : rick <rick.han@yahoo.com>

copyright (c) <2015> <rick>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
**/
#include "LuaNetModule.h"
#include "LuaEngine.h"
#include "TcpClient.h"
#include "NetworkManager.h"
#include "Logger.h"

#define TCPCLIENT_META "tcp_client_meta"
#define LUA_FUNC_NAME_MAX_LEN 200
struct luaTcpClient
{
  TcpClient* _client;
  int64_t    _onConnectFunc;
  int64_t    _onMsgFunc;
  int64_t    _onCloseFunc;
};

void handleClientConnected(const ErrorCode& err, const TcpConnPtr& conn)
{
  luaTcpClient* client = reinterpret_cast<luaTcpClient*>(conn->getUserData());
  LuaEngine& ins = LuaEngine::instance();
  if(err) {
  }
}

static int newTcpClient(lua_State* L)
{
  int args = lua_gettop(L);
  if (args < 2) {
    LOG(ERROR) << "newTcpClient two few argument";
    return 0;
  }

  const char* ip = lua_tostring(L, 1);
  int port = lua_tointeger(L, 2);
  std::string strIp = "127.0.0.1";
  if (ip != nullptr) {
    strIp = ip;
  }

  luaTcpClient* client = (luaTcpClient*)lua_newuserdata(L, sizeof(luaTcpClient));
  client->_client = nullptr;
  client->_onConnectFunc = 0;
  client->_onMsgFunc = 0;
  client->_onCloseFunc = 0;
  luaL_getmetatable(L, TCPCLIENT_META);
  lua_setmetatable(L, -2);
  NetworkManager& manager = NetworkManager::instance();
  client->_client = new TcpClient(manager.getIoService(), strIp, port,
                                  boost::bind(handleClientConnected, _1, _2));
  client->_client->setUserData(reinterpret_cast<void*>(client));
  return 1;
}

#define CHECK_TCP_CLIENT\
    luaTcpClient* client = (luaTcpClient*)luaL_checkudata(L, 1, TCPCLIENT_META);\
    if (client == nullptr || client->_client == nullptr) {\
        return 0;\
    }\

static int disconnect(lua_State* L)
{
  CHECK_TCP_CLIENT;
  client->_client->disconnect();
  return 0;
}

static int connect(lua_State* L)
{
  CHECK_TCP_CLIENT;
  client->_client->connect();
  return 0;
}

static int isconnected(lua_State* L)
{
  CHECK_TCP_CLIENT;
  if (client->_client->isConnected()) {
    lua_pushboolean(L, true);
  } else {
    lua_pushboolean(L, false);
  }
  return 1;
}

#define SET_LUA_CALLBACK(data) \
  CHECK_TCP_CLIENT;\
  const char* luaFunc = lua_tostring(L, 2);\
  if (luaFunc == nullptr || luaFunc == "") {\
    lua_pushboolean(L, false);\
    return 1;\
  }\
  int len = strlen(luaFunc);\
  if (len >= LUA_FUNC_NAME_MAX_LEN) {\
    lua_pushboolean(L, false);\
    return 1;\
  }\
  data = 0;\
  const char* dot = strchr(luaFunc, '.');\
  if (dot != nullptr) {\
    lua_pushlightuserdata(L, (void*)&(data));\
    lua_getglobal(L, luaFunc);\
    if (lua_type(L, -1) != LUA_TFUNCTION) {\
        LOG(ERROR) << "lua function not founded! name=" << luaFunc;\
        lua_pushboolean(L, false);\
        return 1;\
    }\
  } else { \
    int moduleLen = dot - luaFunc;\
    char module[LUA_FUNC_NAME_MAX_LEN];\
    strncpy(module, luaFunc, moduleLen);\
    module[moduleLen] = '\0';\
    int funcLen = len -moduleLen - 1;\
    char func[LUA_FUNC_NAME_MAX_LEN];\
    strncpy(func, dot+1, funcLen);\
    func[funcLen] = '\0';\
    lua_getglobal(L, module);\
    if (lua_type(L, -1) != LUA_TTABLE) {\
      lua_pushboolean(L, false);\
      return 1;\
    }\
    lua_pushlightuserdata(L, (void*)&(client->_onConnectFunc));\
    lua_pushstring(L, func);\
    lua_gettable(L, -3);\
    if (lua_type(L, -1) != LUA_TFUNCTION) {\
      lua_pushboolean(L, false);\
      LOG(ERROR) << "no lua function found! name=" << luaFunc;\
      return 1;\
    }\
  }\
  data = 1;\
  lua_settable(L, LUA_REGISTRYINDEX);\
  lua_pushboolean(L, true);\
  return 1;\

static int setConnectCallback(lua_State* L)
{
  SET_LUA_CALLBACK(client->_onConnectFunc)
}

static int setCloseCallback(lua_State* L)
{
  SET_LUA_CALLBACK(client->_onCloseFunc)
}

static int setMsgCallback(lua_State* L) 
{
  SET_LUA_CALLBACK(client->_onMsgFunc)
}
#undef CHECK_TCP_CLIENT
const static luaL_Reg client_funcs[] =
{
    {"Connect", connect},
    {"Disconnect", disconnect},
    {"IsConnected", isconnected},
    {"SetConnectCallback", setConnectCallback},
    {"SetMsgCallback", setMsgCallback},
    {"SetCloseCallback", setCloseCallback},
    {NULL, NULL},
};

void openTcpClient(lua_State* L)
{
  luaL_newmetatable(L, TCPCLIENT_META);
  lua_pushstring(L, "__index");
  lua_pushvalue(L, -2);
  lua_pushvalue(L, -3);
  luaL_register(L, NULL, client_funcs);
}

const static luaL_Reg net_funcs[] =
{
  {"NewTcpClient", newTcpClient},
  {NULL, NULL},
};

void openLuaNetModule()
{
  LuaEngine& ins = LuaEngine::instance();
  ins.registLib("Net", net_funcs);
  openTcpClient(ins.state());
}
