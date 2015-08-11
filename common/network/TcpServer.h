/**
TcpServer.h
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
#ifndef __TCPSERVER__H__
#define __TCPSERVER__H__

#include "AsioCommon.h"
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <string>
#include "TcpConnection.h"

class TcpServer : boost::noncopyable
{
public:
  TcpServer(IoService& ioService, std::string& ip, int port)
    : _ioService(ioService),
      _ep(boost::asio::ip::address::from_string(ip), port),
      _acceptor(ioService, _ep)
  {
    _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.set_option(boost::asio::ip::tcp::socket::keep_alive(true));
    _acceptor.set_option(boost::asio::ip::tcp::no_delay(true));
    TcpConnPtr conn(new TcpConnection(ioService));
    listen(conn);
  }

  void listen(TcpConnPtr& conn)
  {
    _acceptor.async_accept(conn->socket(),
                           boost::bind(&TcpServer::handleAcceptor,
                                       this,
                                       conn,
                                       boost::asio::placeholders::error));
  }

private:
  void handleAcceptor(TcpConnPtr& conn, const ErrorCode& err) 
  {
    if (err) {
      return;  
    }

    conn->asyncRead(); // 发一个读请求
    conn.reset(new TcpConnection(_ioService));
    listen(conn);
  }
private:
  IoService& _ioService;
  EndPoint   _ep;
  Acceptor   _acceptor;
};

#endif // __TCPSERVER__H__
