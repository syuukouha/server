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
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>
#include "AsioCommon.h"

class TcpConnection : public boost::enable_shared_from_this<TcpConnection>
{
enum {kSendBuffSize = 32 * 1024 /*发送缓存大小*/, kRecvBuffSize = 32 * 1024/*接收缓存大小*/,};
public:
  TcpConnection(IoService& ioService) : _socket(ioService)
  {
    static boost::asio::ip::tcp::no_delay option(true);
    _socket.set_option(option);
  }

  ~TcpConnection()
  {
  }

  void setWriteCallback(WriteCallback wcb) { _writeCb = wcb; }
  void setReadCallback(ReadCallback rcb) { _readCb = rcb; }

  void asyncRead();

  void asyncWrite(const std::string& msg);

  Socket& socket() { return _socket; }

  void asyncConnect(const EndPoint& endpoint, ConnectCallback cb) 
  {
    _socket.async_connect(endpoint, cb);
  }
private:
  void handleRead(const ErrorCode& error, size_t bytesTransferred);
  void handleWrite(const ErrorCode& error, size_t bytesTransferred);
private:
  typedef boost::array<char, kSendBuffSize> SendBuff;
  typedef boost::array<char, kRecvBuffSize> RecvBuff;

  Socket        _socket;
  RecvBuff      _recvBuf;
  WriteCallback _writeCb;
  ReadCallback  _readCb;
};

typedef boost::shared_ptr<TcpConnection> TcpConnPtr;
#endif // __TCPCONNECTION__H__
