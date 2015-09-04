/**
TcpConnection.h
rick <rick.han@yahoo.com>
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
#ifndef __TCPCONNECTION__H__
#define __TCPCONNECTION__H__
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>
#include "AsioCommon.h"
#include "INetPacket.h"

class TcpConnection : public boost::enable_shared_from_this<TcpConnection>
{
enum {kRecvBuffSize = 32 * 1024/*接收缓存大小*/,};
public:
  typedef boost::shared_ptr<TcpConnection> TcpConnPtr;
  typedef boost::function<void (const ErrorCode&, TcpConnPtr)>  ConnectionCallback;
  typedef boost::function<void (const ErrorCode&, const TcpConnPtr& conn)>  CloseCallback;
  typedef boost::function<void (const ErrorCode&)> ConnectCallback; // asio自带的callback
  typedef boost::function<bool (const TcpConnPtr&, const char*, size_t)> ParseCallback;
  typedef boost::function<void (const TcpConnPtr&, const ErrorCode&, size_t)> WriteCallback;
  typedef boost::function<void (const TcpConnPtr&)> PacketCallback;

  TcpConnection(IoService& ioService) : _socket(ioService), _userData(nullptr),
    _netPacket(nullptr)
  {
  }

  ~TcpConnection()
  {
    close();
    if (_netPacket) delete _netPacket;
  }

  Socket& socket() { return _socket; }

  void setWriteCallback(WriteCallback wcb) { _writeCb = wcb; }
  void setConnectCallback(ConnectionCallback ccb) { _connectCb = ccb; }
  void setCloseCallback(CloseCallback clcb) { _closeCb = clcb; }
  void setParseCallback(ParseCallback pcb) { _parseCb = pcb; }
  void setPacketCallback(PacketCallback pcb) { _packetCb = pcb; }
  void asyncRead();
  void asyncWrite(SendBuffPtr buf);
  void asyncWrite(const std::string& content)
  {
    SendBuffPtr buf(new boost::asio::streambuf());
    std::ostream os(buf.get());
    os.write(content.data(), content.size());
    asyncWrite(buf);
  }
  
  /*void asyncWritePacket() 
  {
    if (_netPacket) {
      SendBuffPtr buf(new boost::asio::streambuf());
      if (_netPacket->serialize(buf)) {
        asyncWrite(buf);
      }
    }
  }*/
  void asyncConnect(const EndPoint& endpoint, ConnectCallback cb) 
  {
    _socket.async_connect(endpoint, cb);
  }

  void start();
  void close() 
  { 
    if (_socket.is_open()) {
      ErrorCode err;
      if (_closeCb) {
        _closeCb(err, shared_from_this());
      }
      _socket.close(err); 
    }
  }

  void setConnId(uint64_t id) { _connId = id; }
  uint64_t getConnId() { return _connId; }

  void* getUserData() { return _userData; }
  void setUserData(void* data) { _userData = data; }

  INetPacket* getNetPacket() { return _netPacket; }
  void setNetPacket(INetPacket* packet) { _netPacket = packet; }
private:
  void handleRead(const ErrorCode& error, size_t bytesTransferred);
  void handleWrite(const ErrorCode& error, size_t bytesTransferred);

private:
  typedef boost::array<char, kRecvBuffSize> RecvBuff;

  Socket                _socket;
  RecvBuff              _recvBuf;
  WriteCallback         _writeCb;
  ConnectionCallback    _connectCb;
  CloseCallback         _closeCb;
  ParseCallback         _parseCb;
  PacketCallback        _packetCb;
  uint64_t              _connId;
  void*                 _userData;
  INetPacket*           _netPacket;
};

typedef boost::shared_ptr<TcpConnection> TcpConnPtr;
#endif // __TCPCONNECTION__H__
