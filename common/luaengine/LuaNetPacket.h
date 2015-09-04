/**
FileName : LuaNetPacket.h
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
#ifndef __LUANETPACKET__H__
#define __LUANETPACKET__H__
#include "INetPacket.h"

struct LuaNetPacket : public INetPacket 
{
public:
  enum {kMaxPacketLen = 1024 * 32, };
  ///////////////////////////////////////////////////////////
  /////////////////////// INetPacket ///////////////////////
  // 序列化
  bool serialize(SendBuffPtr& buff) {
    if (_packetLen > kMaxPacketLen || _packetLen > _methodLen) {
      return false;
    }

    std::ostream os(buff.get());
    os.write(reinterpret_cast<char*>(&_packetLen), sizeof(_packetLen));
    os.write(reinterpret_cast<char*>(&_methodLen), sizeof(_methodLen));
    os.write(_method, _methodLen);
    os.write(_content, _packetLen - _methodLen);
    return true;
  }

  // 反序列化
  ssize_t deserialize(const char* data, size_t sz) {
    static uint32_t packetHeadLen = sizeof(_packetLen)
                                    + sizeof(_methodLen);
    if (sz < packetHeadLen) {
      return 0;
    }

    _packetLen = *reinterpret_cast<const uint32_t*>(data);
    _methodLen = *reinterpret_cast<const uint8_t*>(data + sizeof(_packetLen));
    if (_packetLen < _methodLen) {
      return -1;
    }
    return 0;
  }

public:
  inline uint32_t packetLen()   { return _packetLen; }
  inline uint8_t  methodLen()   { return _methodLen; }
  inline char*    method()      { return _method; }
  inline char*    content()     { return _content; }

  void makePacket(char* method, char* content, size_t contentLen) {
    _method     = method;
    _content    = content;
    _packetLen  = contentLen;
    _methodLen  = strlen(method);
    _packetLen  += _methodLen;
  }

  void reset() {
    _method     = nullptr;
    _content    = nullptr;
    _packetLen  = 0;
    _methodLen  = 0;
  }
private:
  uint32_t      _packetLen; // 包总长
  uint8_t       _methodLen; // 函数名长
  char*         _method;    // 函数名
  char*         _content;   // 包体
};

#endif // __LUANETPACKET__H__
