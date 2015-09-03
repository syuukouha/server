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
#include "TcpServer.h"
#include "NetworkManager.h"
#include "Logger.h"
#include <ext/hash_map>
#define LUANET_META "__lua__net__meta__"
#define LUA_FUNC_NAME_MAX_LEN 200

static int connect(lua_State*);
static int disconnect(lua_State*);
static int isconnected(lua_State*);
static int setConnectCallback(lua_State*);
static int setMsgCallback(lua_State*);
static int setCloseCallback(lua_State*);
static int setWriteCallback(lua_State*);
static int close(lua_State*);
const static luaL_Reg netmeta_funcs[] =
{
    {"Connect", connect},
    {"Disconnect", disconnect},
    {"IsConnected", isconnected},
    {"SetConnectCallback", setConnectCallback},
    {"SetMsgCallback", setMsgCallback},
    {"SetCloseCallback", setCloseCallback},
    {"SetWriteCallback", setWriteCallback},
    {"Close", close},
    {NULL, NULL},
};

static int newTcpClient(lua_State*);
static int newTcpServer(lua_State*);
static int start(lua_State*);
static int stop(lua_State*);
static int send(lua_State*);
static int closeConn(lua_State*);
const static luaL_Reg net_funcs[] =
{
  {"NewTcpClient", newTcpClient},
  {"NewTcpServer", newTcpServer},
  {"Start", start},
  {"Stop", stop},
  {"Send", send},
  {"CloseConn", closeConn},
  {NULL, NULL},
};

void openLuaNetModule()
{
  LuaEngine& ins = LuaEngine::instance();
  lua_State* L = ins.state();
  luaL_newmetatable(L, LUANET_META);
  lua_pushstring(L, "__index");
  lua_pushvalue(L, -2);
  lua_pushvalue(L, -3);
  luaL_register(L, NULL, netmeta_funcs);
  luaL_register(L, "Net", net_funcs);
}


enum LuaNetDataType { Type_Client, Type_Server, };
struct LuaNetData
{
  uint64_t  _id;
  int       _dataType;
  int64_t   _connectFunc;
  int64_t   _msgFunc;
  int64_t   _closeFunc;
  int64_t   _writeFunc;
  void*     _extra;
};

static uint64_t _nextLuaNetDataId = 0;
typedef __gnu_cxx::hash_map<uint64_t, LuaNetData*> NetMap;
typedef NetMap::iterator NetIter;
NetMap _gNetMap;

void handleConnected(const ErrorCode&, TcpConnPtr);

#define CHECK_AND_CREATE_NEW_LUANET(type) \
  int args = lua_gettop(L);\
  if (args < 2) {\
    LOG(ERROR) << "two few argument";\
    return 0;\
  }\
  const char* ip = lua_tostring(L, 1);\
  int port = lua_tointeger(L, 2);\
  std::string strIp = "127.0.0.1";\
  if (ip != nullptr) {\
    strIp = ip;\
  }\
  LuaNetData* data = static_cast<LuaNetData*>(lua_newuserdata(L,\
                                              sizeof(LuaNetData)));\
  if (data == nullptr) {\
    LOG(ERROR) << "memory not enough!!!";\
    return 0;\
  }\
  ++_nextLuaNetDataId;\
  data->_id          = _nextLuaNetDataId;\
  data->_dataType    = type;\
  data->_connectFunc = 0;\
  data->_msgFunc     = 0;\
  data->_closeFunc   = 0;\
  data->_writeFunc   = 0;\
  data->_extra       = nullptr;\
  _gNetMap[data->_id] = data;\
  luaL_getmetatable(L, LUANET_META);\
  lua_setmetatable(L, -2);\

static int newTcpClient(lua_State* L)
{
  CHECK_AND_CREATE_NEW_LUANET(Type_Client);
  NetworkManager& manager = NetworkManager::instance();
  TcpClient* client = new TcpClient(manager.getIoService(), strIp, port,
                                    boost::bind(handleConnected, _1, _2));
  client->setUserData(static_cast<void*>(data));
  data->_extra = static_cast<void*>(client);
  return 1;
}

static int newTcpServer(lua_State* L)
{
  CHECK_AND_CREATE_NEW_LUANET(Type_Server);
  NetworkManager& manager = NetworkManager::instance();
  TcpServer* server = new TcpServer(manager.getIoService(), strIp, port,
                                    boost::bind(handleConnected, _1, _2));
  server->setUserData(static_cast<void*>(data));
  data->_extra = static_cast<void*>(server);
  return 1;
}
#undef CHECK_AND_CREATE_NEW_LUANET

static int start(lua_State* L) 
{
  IoService& ioService = NetworkManager::instance().getIoService();
  ioService.run();
  return 0;
}

static int stop(lua_State* L)
{
  IoService& ioService = NetworkManager::instance().getIoService();
  ioService.stop();
  return 0;
}

static int send(lua_State* L)
{
  return 1;
}

static int closeConn(lua_State* L) 
{
  return 0;
}
#define CHECK_NET_META\
  LuaNetData* data= static_cast<LuaNetData*>(luaL_checkudata(L, 1, LUANET_META));\
  if (data == nullptr || data->_extra == nullptr) {\
    return 0;\
  }\

#define CHECK_TCP_CLIENT\
  CHECK_NET_META\
  if (data->_dataType != Type_Client) {\
    return 0;\
  }\
  TcpClient* client = static_cast<TcpClient*>(data->_extra);

static int disconnect(lua_State* L)
{
  CHECK_TCP_CLIENT;
  client->disconnect();
  return 0;
}

static int connect(lua_State* L)
{
  CHECK_TCP_CLIENT;
  client->connect();
  return 0;
}

static int isconnected(lua_State* L)
{
  CHECK_TCP_CLIENT;
  if (client->isConnected()) {
    lua_pushboolean(L, true);
  } else {
    lua_pushboolean(L, false);
  }
  return 1;
}

#define SET_LUA_CALLBACK(funcref) \
  CHECK_NET_META;\
  const char* luaFunc = lua_tostring(L, 2);\
  if (luaFunc == nullptr) {\
    lua_pushboolean(L, false);\
    return 1;\
  }\
  int len = strlen(luaFunc);\
  if (len >= LUA_FUNC_NAME_MAX_LEN) {\
    lua_pushboolean(L, false);\
    return 1;\
  }\
  funcref = 0;\
  const char* dot = strchr(luaFunc, '.');\
  if (dot != nullptr) {\
    lua_pushlightuserdata(L, (void*)&(funcref));\
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
    lua_pushlightuserdata(L, (void*)&(funcref));\
    lua_pushstring(L, func);\
    lua_gettable(L, -3);\
    if (lua_type(L, -1) != LUA_TFUNCTION) {\
      lua_pushboolean(L, false);\
      LOG(ERROR) << "no lua function found! name=" << luaFunc;\
      return 1;\
    }\
  }\
  funcref = 1;\
  lua_settable(L, LUA_REGISTRYINDEX);\
  lua_pushboolean(L, true);\
  return 1;\

static int setConnectCallback(lua_State* L)
{
  SET_LUA_CALLBACK(data->_connectFunc)
}

static int setCloseCallback(lua_State* L)
{
  SET_LUA_CALLBACK(data->_closeFunc)
}

static int setMsgCallback(lua_State* L) 
{
  SET_LUA_CALLBACK(data->_msgFunc)
}

static int setWriteCallback(lua_State* L) 
{
  SET_LUA_CALLBACK(data->_writeFunc)
}

static int close(lua_State* L) 
{
  CHECK_NET_META;
  if (data->_dataType == Type_Client) {
    TcpClient* client = static_cast<TcpClient*>(data->_extra);
    client->disconnect();
  } else {
    TcpServer* server = static_cast<TcpServer*>(data->_extra);
    server->stop();
  }
  return 0;
}
#undef SET_LUA_CALLBACK
#undef CHECK_TCP_CLIENT
#undef CHECK_NET_META


typedef __gnu_cxx::hash_map<uint64_t, TcpConnPtr> ConnMaps;
typedef ConnMaps::iterator ConnIter;
ConnMaps _gConnMaps;
static uint64_t _nextConnId = 0;

// 连接被断开
void handleClose(const ErrorCode& err, const TcpConnPtr& conn)
{
  uint64_t connId = conn->getConnId();
  ConnIter it = _gConnMaps.find(connId);
  _gConnMaps.erase(it);
  LuaNetData* data = static_cast<LuaNetData*>(conn->getUserData());
  if (data == nullptr) {
    return;
  }
  LuaEngine& ins = LuaEngine::instance();
  LuaValueArray inputArgs;
  inputArgs.push_back(LuaValue::ccobjectValue(reinterpret_cast<void*>(connId)));
  ins.callLuaFunction(&(data->_closeFunc), &inputArgs);
}

// 包解析
bool handleRead(const TcpConnPtr& conn, const char* data, size_t sz) 
{
  return true;
}

void handleWrite(const TcpConnPtr& conn, const ErrorCode& err, size_t sz)
{
  LuaNetData* data = static_cast<LuaNetData*>(conn->getUserData());
  if (data == nullptr) {
    return;
  }
  uint64_t connId = conn->getConnId();
  LuaEngine& ins = LuaEngine::instance();
  LuaValueArray inputArgs;
  inputArgs.push_back(LuaValue::ccobjectValue(reinterpret_cast<void*>(connId)));
  inputArgs.push_back(LuaValue::intValue(sz));
  ins.callLuaFunction(&(data->_writeFunc), &inputArgs);
}

// 新连接建立
void handleConnected(const ErrorCode& err, TcpConnPtr conn)
{
  LuaNetData* data = static_cast<LuaNetData*>(conn->getUserData());
  if (data == nullptr || data->_extra == nullptr) {
    return;
  }

  LuaEngine& ins = LuaEngine::instance();
  LuaValueArray inputArgs;
  if (err) {
    inputArgs.push_back(LuaValue::booleanValue(false));
  } else {
    ++_nextConnId;
    conn->setConnId(_nextConnId);
    conn->setCloseCallback(boost::bind(handleClose, _1, _2));
    conn->setWriteCallback(boost::bind(handleWrite, _1, _2, _3));
    conn->setParseCallback(boost::bind(handleRead, _1, _2, _3));
    inputArgs.push_back(LuaValue::booleanValue(true));
    inputArgs.push_back(LuaValue::ccobjectValue(reinterpret_cast<void*>(_nextConnId)));
    _gConnMaps[_nextConnId] = conn;
  }
  ins.callLuaFunction(&(data->_connectFunc), &inputArgs);
}
