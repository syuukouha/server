/**
FileName : LuaEngine.cpp
Author   : rick <rick.han@yahoo.com>

Copyright (c) <2015> <rick>

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
#include "LuaEngine.h"
#include "Logger.h"
#include "lfs.h"
#include "LuaNetModule.h"
#include "LuaSysModule.h"
// lua错误处理函数
static int luaErrorFunc(lua_State* L)
{
  lua_Debug debug = {};
  std::string stackTrace = "";
  if (lua_gettop(L) > 0) {
    stackTrace += lua_tostring(L, -1);
  }
  stackTrace += "\nstack traceback";

  char buff[513] = {0};
  int stackLevel = 1;
  while(lua_getstack(L, stackLevel++, &debug)) {
    lua_getinfo(L, "Snl", &debug);
    snprintf(buff, 512, "\n\t%s", debug.short_src);
    int buffLen = strlen(buff);
    if (*debug.namewhat != '\0') {
      snprintf(buff+buffLen, 512-buffLen, " in function \'%s\'", debug.name);
    } else {
      if (*debug.what == 'm') {
        snprintf(buff+buffLen, 512-buffLen, " in main chunk ");
      } else if(*debug.what == 't' || *debug.what == 'C') {
        snprintf(buff+buffLen, 512-buffLen, " ? ");
      } else {
        snprintf(buff+buffLen, 512-buffLen, " in function <%s:%d>",
                 debug.short_src, debug.linedefined);
      }
    }

    stackTrace += buff;
  }

  LOG(ERROR) << stackTrace;
  return 0;
}

LuaEngine::LuaEngine(): _L(luaL_newstate())
{
  luaL_openlibs(_L);
  luaopen_lfs(_L);
  openLuaNetModule();
  openLuaSysModule();
}

LuaEngine::~LuaEngine()
{
}

void LuaEngine::addSearchPath(const char* path)
{
  lua_getglobal(_L, "package");
  lua_getfield(_L, -1, "path"); // package.path
  const char* currPath = lua_tostring(_L, -1);
  lua_pushfstring(_L, "%s;%s/?.lua", currPath, path);
  lua_setfield(_L, -3, "path");
  lua_pop(_L, -2);
}

void LuaEngine::loadFile(const char* fileName)
{
  luaL_dofile(_L, fileName);
  lua_pcall(_L, 0, 0, 0);
}

void LuaEngine::registLib(const char* libname, const luaL_Reg* lib)
{
  luaL_register(_L, libname, lib);
}

bool LuaEngine::callLuaFunction(const char* methodName, LuaValueArray* input, 
                                LuaValueArray* output, int numRet)
{
  int oldTop = lua_gettop(_L);
  lua_pushcfunction(_L, luaErrorFunc); // 错误处理函数
  lua_getglobal(_L, methodName);
  int args = 0;
  if (input != nullptr) {
     LuaValueArrayIterator it = input->begin();
     for (; it != input->end(); ++it) {
       args++;
       it->pushToStack(_L);
     }
  }
  int result = lua_pcall(_L, args, numRet, 1);
  if (result) {
    const char* errorMsg = lua_tostring(_L, -1);
    LOG(ERROR) << errorMsg;
    lua_settop(_L, oldTop);
    return false;
  }

  // 填充返回值
  if (output != nullptr && numRet > 0) {
    for (int i=0; i<numRet; ++i) {
      output->push_front(LuaValue::toValue(_L, -1));
      lua_pop(_L, 1);
    }
  }

  lua_settop(_L, oldTop);
  return true;
}

bool LuaEngine::callLuaFunction(int64_t* func, LuaValueArray* input,
                                LuaValueArray* output, int numRet) 
{
  if (func == nullptr || *func == 0) {
    return false;
  }

  lua_settop(_L, 0);
  lua_pushcfunction(_L, luaErrorFunc);

  lua_pushlightuserdata(_L, reinterpret_cast<void*>(func));
  lua_gettable(_L, LUA_REGISTRYINDEX);
  if (lua_type(_L, -1) == LUA_TFUNCTION) {
    int args = 0;
    if (input != nullptr) {
      LuaValueArrayIterator it = input->begin();
      for (; it != input->end(); ++it) {
        args++;
        it->pushToStack(_L);
      }
    }
    int result = lua_pcall(_L, args, numRet, 1);
    if (result) {
      const char* errorMsg = lua_tostring(_L, -1);
      LOG(ERROR) << errorMsg;
      lua_settop(_L, 0);
      return false;
    }
    // 填充返回值
    if (output != nullptr && numRet > 0) {
      for (int i=0; i<numRet; ++i) {
        output->push_front(LuaValue::toValue(_L, -1));
        lua_pop(_L, 1);
      }
    }
  }
  lua_settop(_L, 0);
  return true;
}

void LuaEngine::executeLuaCode(const char* luaCode) 
{
  luaL_loadstring(_L, luaCode);
  lua_pcall(_L, 0, 0, 0);
}
