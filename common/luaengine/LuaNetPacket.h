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
#include <sstream>
#include <boost/array.hpp>

/// 消息包格式：
/// 总长(3bytes) + 函数名长(1byte) + 函数名(0-255bytes) + 消息体(nbytes)
struct LuaNetPacket : public INetPacket 
{
public:
  enum {kMaxPacketLen = 1024 * 32, kHeadLen = sizeof(uint32_t),};
  enum PacketState {eState_ParseHead = 0, eState_ParseData, eState_ReadComplete,};
  typedef boost::array<char, kHeadLen> PacketHeadBuffer;

  LuaNetPacket(): _data(std::ios_base::in|std::ios_base::out|std::ios_base::binary),
    _state(eState_ParseHead), _needSize(kHeadLen)
  {
  }

  ///////////////////////////////////////////////////////////
  /////////////////////// INetPacket ///////////////////////
  // 序列化
  bool serialize(SendBuffPtr& buff) {
    return true;
  }

  // 反序列化
  ssize_t deserialize(const char* data, size_t sz) {
    ssize_t readSize = 0;
    while (true) {
      switch (_state) {
      case eState_ParseHead:
        if (sz >= _needSize) {
          size_t startPos = _head.size() - _needSize;
          for (size_t i=0; i<_needSize; ++i) {
            _head[i+startPos] = data[i];
          }
          data += _needSize;
          sz -= _needSize;
          readSize += _needSize;
          _needSize = packetLen() - kHeadLen;
          _state = eState_ParseData;
          uint8_t mlen = methodLen();
          if (_needSize >= kMaxPacketLen || (_needSize < (mlen+sizeof(uint32_t))
                      || mlen == 0)) {
            return -1; // 错误的包，断开连接
          }
        } else {
          size_t needLen = _head.size() - _needSize;
          for (size_t i=0; i<sz; ++i) {
            _head[needLen+i] = data[i];
          }
          readSize += sz;
          _needSize -= sz;
          return readSize;
        }
        break;
      case eState_ParseData:
        if (sz >= _needSize) {
          _data.write(data, _needSize);
          readSize += _needSize;
          _state = eState_ReadComplete;
          return readSize;
        } else {
          _data.write(data, sz);
          readSize += sz;
          return readSize;
        }
        break;
      default:
        return readSize;
      }
    }

    return 0;
  }

  bool prepared() { return _state == eState_ReadComplete; }
  void reset() {
    _state    = eState_ParseHead;
    _needSize = kHeadLen;
    for (size_t i=0; i<_head.size(); ++i) {
      _head[i] = 0;
    }
    _data.str("");
    _data.clear();
  }
public:
  // 包总长
  inline uint32_t packetLen()   
  {
    uint32_t data = *reinterpret_cast<const uint32_t*>(_head.data());
    return data >> 8;
  }

  // 函数名长
  inline uint8_t methodLen()   
  {
    uint32_t data = *reinterpret_cast<const uint32_t*>(_head.data());
    return data & 0xFF;
  }

  inline std::string content()
  {
    return _data.str();
  } 

  SendBuffPtr makePacket(const char* method, const char* content,
                         size_t contentLen) 
  {
    if (method == nullptr || content == nullptr) 
    {
      boost::asio::streambuf* p = nullptr;
      return SendBuffPtr(p);
    }

    uint32_t len = strlen(method);
    if (len >= 0xFF || (len+contentLen+kHeadLen) >= kMaxPacketLen) {
      boost::asio::streambuf* p = nullptr;
      return SendBuffPtr(p);
    }

    uint32_t totalLen = len + contentLen + kHeadLen;
    totalLen = totalLen << 8;
    totalLen += len;
    std::ostringstream output;
    output.write(reinterpret_cast<const char*>(&totalLen), sizeof(totalLen));
    output.write(method, len);
    output.write(content, contentLen);
    SendBuffPtr buf(new boost::asio::streambuf());
    std::ostream os(buf.get());
    std::string data = output.str();
    os.write(data.data(), data.size());
    return buf;
  }

private:
  PacketHeadBuffer  _head;
  std::stringstream _data;
  uint32_t          _state;
  uint32_t          _needSize;
};

#endif // __LUANETPACKET__H__
