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

#define TCPCLIENT_META "tcp client meta"

struct luaTcpClient
{
  TcpClient* _client;
};

void handleConnected(const ErrorCode& err, const TcpConnPtr& conn)
{
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
  client->_client = NULL;
  luaL_getmetatable(L, TCPCLIENT_META);
  lua_setmetatable(L, -2);
  NetworkManager& manager = NetworkManager::instance();
  client->_client = new TcpClient(manager.getIoService(), strIp, port,
                                  boost::bind(handleConnected, _1, _2));

  return 1;
}

static int disconnect(lua_State* L)
{
  luaTcpClient* client = (luaTcpClient*)luaL_checkudata(L, 1, TCPCLIENT_META);
  if (client != nullptr && client->_client != nullptr) {
    client->_client->disconnect();
  }

  return 0;
}

static int connect(lua_State* L)
{
  luaTcpClient* client = (luaTcpClient*)luaL_checkudata(L, 1, TCPCLIENT_META);
  if (client != nullptr && client->_client != nullptr) {
    client->_client->connect();
  }

  return 0;
}

static int isconnected(lua_State* L) 
{
  luaTcpClient* client = (luaTcpClient*)luaL_checkudata(L, 1, TCPCLIENT_META);
  if (client != nullptr && client->_client != nullptr) {
    if (client->_client->isConnected()) {
      lua_pushboolean(L, true);
    } else {
      lua_pushboolean(L, false);
    }
    return 1;
  }
  lua_pushboolean(L, false);
  return 1;
}

void openTcpClient(lua_State* L) 
{
  luaL_newmetatable(L, TCPCLIENT_META);

  // method table
  lua_newtable(L);
  lua_pushcfunction(L, disconnect);
  lua_setfield(L, -2, "disconnect");
  lua_pushcfunction(L, connect);
  lua_setfield(L, -2, "connect");
  lua_pushcfunction(L, isconnected);
  lua_setfield(L, -2, "isconnected");
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
