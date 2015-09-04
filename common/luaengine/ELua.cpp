/**
FileName : ELua.cpp
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

#include "LuaEngine.h"
#include <iostream>

int main(int argc, char** argv) 
{
  if (argc < 2) {
    std::cout << "too few arguments!!!!!" << std::endl;
    return 0;
  }
  LuaEngine& ins = LuaEngine::instance();
  lua_State* L   = ins.state();
  lua_getglobal(L, "Sys");
  if (lua_type(L, -1) != LUA_TTABLE) {
    std::cout << "sys module not found!!!!" << std::endl;
    return 0;
  }

  // Sys.Argc
  lua_pushnumber(L, argc);
  lua_setfield(L, -2, "Argc"); 

  // Sys.Argv
  lua_newtable(L);
  for (int i=0; i<argc; ++i) {
    lua_pushstring(L, argv[i]);
    lua_rawseti(L, -2, i);
  }
  lua_setfield(L, -2, "Argv");

  ins.loadFile(argv[1]);
  return 0;
}
