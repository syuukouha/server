/**
FileName : LuaLogModule.cpp
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
#include "LuaLogModule.h"
#include "Logger.h"
#include "LuaEngine.h"

static int initGoogleLogging(lua_State*);
static int shutdownLogging(lua_State*);
static int setLogDestination(lua_State*);
static int setLogDir(lua_State*);
static int setLogBufSecs(lua_State*);
static int error(lua_State*);
static int info(lua_State*);
static int warn(lua_State*);
static int fatal(lua_State*);
static const luaL_Reg logFuncs[] = 
{
  {"InitGoogleLogging", initGoogleLogging},
  {"ShutdownLogging", shutdownLogging},
  {"SetLogDestination", setLogDestination},
  {"SetLogDir", setLogDir},
  {"SetLogBufSecs", setLogBufSecs},
  {"Error", error},
  {"Info", info},
  {"Warn", warn},
  {"Fatal", fatal},
  {NULL, NULL},
};

void openLuaLogModule()
{
  LuaEngine& inst = LuaEngine::instance();
  inst.registLib("Log", logFuncs);
}

static int initGoogleLogging(lua_State* L) 
{
  if (!lua_isstring(L, 1)) {
    return 0;
  }

  google::InitGoogleLogging(lua_tostring(L, 1));
  return 0;
}

static int shutdownLogging(lua_State* L) 
{
  google::ShutdownGoogleLogging();
  return 0;
}

static int setLogDestination(lua_State* L) 
{
  if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
    return 0;
  }

  google::SetLogDestination((int)lua_tonumber(L, 1),
                            lua_tostring(L, 2));
  return 0;
}

static int setLogDir(lua_State* L) 
{
  if (!lua_isstring(L, 1)) {
    return 0;
  }

  FLAGS_log_dir = lua_tostring(L, 1);
  return 0;
}

static int setLogBufSecs(lua_State* L) 
{
  if (!lua_isnumber(L, 1)) {
    return 0;
  }

  FLAGS_logbufsecs = (int)lua_tonumber(L, 1);
  return 0;
}

#define LOG_STR_(statement)\
    const char* str = lua_tostring(L, 1);\
    if (str) statement << str;\
    return 0;


static int error(lua_State* L) 
{
  LOG_STR_(LOG(ERROR))
}

static int info(lua_State* L) 
{
  LOG_STR_(LOG(INFO))
}

static int warn(lua_State* L) 
{
  LOG_STR_(LOG(WARNING))
}

static int fatal(lua_State* L) 
{
  LOG_STR_(LOG(FATAL))
}

#undef LOG_STR_
