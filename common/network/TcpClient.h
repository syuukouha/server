/**
TcpClient.h
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
#ifndef __TCPCLIENT__H__
#define __TCPCLIENT__H__

#include "AsioCommon.h"
#include "TcpConnection.h"
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <string>
class TcpClient : public boost::noncopyable
{
enum {eState_NotConnected, eState_Connecting, eState_Connected, };
public:
  TcpClient(IoService& ioService, std::string ip, int port)
    : _conn(ioService), _state(eState_NotConnected),
      _ep(boost::asio::ip::address::from_string(ip), port)
  {}

  ~TcpClient() {}

  // 连接建立回调
  void setConnectCallback(ConnectCallback ccb) { _connectCb = ccb; }
  // 读完成回调
  void setReadCallback(ReadCallback rcb) { _conn.setReadCallback(rcb); }
  // 写完成回调
  void setWriteCallback(WriteCallback wcb) { _conn.setWriteCallback(wcb); }

  // 建立连接
  void connect()
  {
    _state = eState_Connecting;
    _conn.asyncConnect(_ep, boost::bind(&TcpClient::handleConnect,
                                        this,
                                        boost::asio::placeholders::error));
  }

  // 发送消息到服务端
  void send(const std::string& msg);

  // 读取消息
  void read();
private:
  void handleConnect(const ErrorCode& error)
  {
    if (!error) {
      _state = eState_Connected;
    } else {
      connect(); // 重连
    }

    if (_connectCb) {
      _connectCb(error);
    }
  }
private:
  TcpConnection     _conn;
  int               _state;
  EndPoint          _ep;
  ConnectCallback   _connectCb;
};


#endif // __TCPCLIENT__H__
