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
  _socket.async_read_some(boost::asio::buffer(_recvBuf), 
                          boost::bind(&TcpConnection::handleRead,
                                      shared_from_this(),
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

void TcpConnection::asyncWrite(SendBuffPtr buff)
{
  //  _sendBuf.
  boost::asio::async_write(_socket, *buff, 
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
  if (_connectCb) {
    ErrorCode err;
    _connectCb(err, shared_from_this());
  }
}

void TcpConnection::handleRead(const ErrorCode& error, size_t bytesTransferred)
{
  if (error) {
    close();
    return;
  }

  if (_netPacket) {
    ssize_t packetSize = _netPacket->deserialize(_recvBuf.data(), bytesTransferred);
    if (packetSize > 0) {
      if (_packetCb) {
        _packetCb(shared_from_this());
      }
      asyncRead();
    } else if (packetSize < 0) {
      close();
    } else {
    }
  }
}

void TcpConnection::handleWrite(const ErrorCode& error, size_t bytesTransferred)
{
  if (_writeCb) {
    _writeCb(shared_from_this(), error, bytesTransferred);
  }
}
