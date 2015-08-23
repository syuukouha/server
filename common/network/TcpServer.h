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
#include "Logger.h"
#include <ext/hash_map>
class TcpServer : boost::noncopyable
{
public:
  typedef TcpConnection::ConnectionCallback ConnectionCallback;
  typedef TcpConnection::CloseCallback CloseCallback;

  TcpServer(IoService& ioService, std::string& ip, int port, ConnectionCallback cb)
    : _ioService(ioService),
      _ep(boost::asio::ip::address::from_string(ip), port),
      _acceptor(ioService, _ep), _connectCb(cb)
  {
    _closeCb = boost::bind(&TcpServer::handleDisconnect, this, _1, _2);
    _nextConnId = 0;
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

  void stop() { _acceptor.close(); }

private:
  void handleAcceptor(TcpConnPtr& conn, const ErrorCode& err) 
  {
    if (err) {
      LOG(ERROR) << "accept error" ;
      return;  
    }

    conn->setConnId(_nextConnId);
    _connMaps[_nextConnId] = conn;
    conn->setConnectCallback(_connectCb);
    conn->setCloseCallback(_closeCb);
    conn->start(); // 发一个读请求
    conn.reset(new TcpConnection(_ioService));
    listen(conn);
  }

  void handleDisconnect(const ErrorCode& err,const TcpConnPtr& conn) {
    uint64_t id = conn->getConnId();
    ConnIter it = _connMaps.find(id);
    if (it == _connMaps.end()) {
      return;
    }
    _connMaps.erase(it);
  }
private:
  typedef __gnu_cxx::hash_map<uint64_t, TcpConnPtr> ConnMaps;
  typedef ConnMaps::iterator ConnIter;

  IoService&         _ioService;
  EndPoint           _ep;
  Acceptor           _acceptor;
  ConnectionCallback _connectCb;
  CloseCallback      _closeCb;
  uint64_t           _nextConnId; // 下一个连接的id
  ConnMaps           _connMaps;
};

#endif // __TCPSERVER__H__
