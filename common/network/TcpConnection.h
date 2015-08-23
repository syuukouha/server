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

class TcpConnection : public boost::enable_shared_from_this<TcpConnection>
{
enum {kRecvBuffSize = 32 * 1024/*接收缓存大小*/,};
public:
  typedef boost::shared_ptr<TcpConnection> TcpConnPtr;
  typedef boost::function<void (const ErrorCode&, const TcpConnPtr& conn)>  ConnectionCallback;
  typedef boost::function<void (const ErrorCode&, const TcpConnPtr& conn)>  CloseCallback;
  typedef boost::function<void (const ErrorCode&)> ConnectCallback; // asio自带的callback
  typedef boost::function<bool (const TcpConnPtr& conn, const char*, size_t)> ParseCallback;

  TcpConnection(IoService& ioService) : _socket(ioService)
  {
  }

  ~TcpConnection()
  {
    close();
  }

  Socket& socket() { return _socket; }

  void setWriteCallback(WriteCallback wcb) { _writeCb = wcb; }
  void setReadCallback(ReadCallback rcb) { _readCb = rcb; }
  void setConnectCallback(ConnectionCallback ccb) { _connectCb = ccb; }
  void setCloseCallback(CloseCallback clcb) { _closeCb = clcb; }
  void setParseCallback(ParseCallback pcb) { _parseCb = pcb; }

  void asyncRead();
  void asyncWrite(SendBuffPtr buf);
  void asyncWrite(const std::string& content)
  {
    SendBuffPtr buf(new boost::asio::streambuf());
    std::ostream os(buf.get());
    os.write(content.data(), content.size());
    asyncWrite(buf);
  }
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
private:
  void handleRead(const ErrorCode& error, size_t bytesTransferred);
  void handleWrite(const ErrorCode& error, size_t bytesTransferred);

private:
  typedef boost::array<char, kRecvBuffSize> RecvBuff;

  Socket                _socket;
  RecvBuff              _recvBuf;
  WriteCallback         _writeCb;
  ReadCallback          _readCb;
  ConnectionCallback    _connectCb;
  CloseCallback         _closeCb;
  ParseCallback         _parseCb;
};

typedef boost::shared_ptr<TcpConnection> TcpConnPtr;
#endif // __TCPCONNECTION__H__
