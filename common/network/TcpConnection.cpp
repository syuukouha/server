/**
TcpConnection.cpp
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
#include "TcpConnection.h"
#include <boost/bind.hpp>

void TcpConnection::asyncRead()
{
  boost::asio::async_read(_socket, boost::asio::buffer(_recvBuf), 
                          boost::bind(&TcpConnection::handleRead,
                                      shared_from_this(),
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

void TcpConnection::asyncWrite(const std::string& msg)
{
  if (msg.size() > kSendBuffSize) {
      return;
  }

  //  _sendBuf.
  boost::asio::async_write(_socket, boost::asio::buffer(msg), 
                           boost::bind(&TcpConnection::handleWrite,
                                       shared_from_this(),
                                       boost::asio::placeholders::error,
                                       boost::asio::placeholders::bytes_transferred));
}

void TcpConnection::start()
{
  _socket.set_option(boost::asio::ip::tcp::socket::keep_alive(true));
  _socket.set_option(boost::asio::ip::tcp::no_delay(true));
  _socket.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  asyncRead();
}

void TcpConnection::handleRead(const ErrorCode& error, size_t bytesTransferred)
{
  if (_readCb) {
    _readCb(error, bytesTransferred);
  }
}

void TcpConnection::handleWrite(const ErrorCode& error, size_t bytesTransferred)
{
  if (_writeCb) {
    _writeCb(error, bytesTransferred);
  }
}
