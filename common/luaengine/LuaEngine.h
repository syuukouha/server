/**
FileName : LuaEngine.h
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
#ifndef __LUAENGINE__H__
#define __LUAENGINE__H__
#include "lua.hpp"
#include <boost/noncopyable.hpp>
#include "CCLuaValue.h"
class LuaEngine : public boost::noncopyable
{
public:
  static LuaEngine& instance()
  {
    static LuaEngine luaEngine;
    return luaEngine;
  }

  lua_State* state() { return _L; }

  // 添加路径
  void addSearchPath(const char* path);

  // 加载lua代码文件
  void loadFile(const char* fileName);

  // 注册c/c++库
  void registLib(const char* libname, const luaL_Reg* lib);

  // 调用lua函数
  bool callLuaFunction(const char* methodName, LuaValueArray* input = nullptr, 
                       LuaValueArray* output = nullptr, int numRet = 0);

  // 调用lua函数
  bool callLuaFunction(int64_t* func, LuaValueArray* input = nullptr,
                       LuaValueArray* output = nullptr, int numRet = 0);
  // 执行lua代码
  void executeLuaCode(const char* luaCode);

private:
  LuaEngine();
  ~LuaEngine();

private:
  lua_State*    _L;
};



#endif // __LUAENGINE__H__
