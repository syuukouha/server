/**
FileName : TestLua.cpp
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
#include <iostream>
#include "LuaNetModule.h"
// cpp func
static int cppFunc(lua_State* L)
{
  int args = lua_gettop(L);
  std::cout << "num of args passed by lua=" << args << std::endl;
  lua_pushboolean(L, true); // 返回值
  return 1;
}

static const luaL_Reg funcs[] = 
{
  {"CppFunc", cppFunc},
  {NULL, NULL},
};

int main(int argc, char** argv)
{
  LuaEngine& ins = LuaEngine::instance();
  ins.openLibs();
  ins.registLib("Test", funcs);
  ins.loadFile("main.lua");
  ins.callLuaFunction("CalledByCpp");
  LuaValueArray paramList;
  paramList.push_back(LuaValue::intValue(10));
  paramList.push_back(LuaValue::stringValue("For Test"));
  ins.callLuaFunction("TwoArgs", &paramList);
  LuaValueArray retList;
  ins.callLuaFunction("TwoArgsAndRets", &paramList, &retList, 2);
  for (LuaValueArrayIterator it = retList.begin(); it != retList.end(); ++it) 
  {
    switch(it->getType())
    {
    case LuaValueTypeInt:
        std::cout <<"ret: int " << it->intValue();
        break;
    case LuaValueTypeBoolean:
        std::cout << "ret: boolean " << it->booleanValue();
        break;
    case LuaValueTypeFloat:
        std::cout << "ret: float " << it->floatValue();
        break;
    case LuaValueTypeObject:
        std::cout << "ret: object(addr) " << it->ccobjectValue();
        break;
    case LuaValueTypeString:
        std::cout << "ret: string " << it->stringValue();
        break;
    default:
        std::cout << "ret: unknowtype " << it->getType();
        break;
    }
    std::cout << std::endl;
  }
  return 0;
}
