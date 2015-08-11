/**
AsioCommon.h
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
#ifndef __ASIOCOMMON__H__
#define __ASIOCOMMON__H__

#include <boost/asio.hpp>
#include <boost/function.hpp>

typedef boost::asio::io_service         IoService;
typedef boost::asio::ip::tcp::socket    Socket;
typedef boost::asio::ip::tcp::endpoint  EndPoint;
typedef boost::asio::ip::tcp::acceptor  Acceptor;
typedef boost::system::error_code       ErrorCode;

typedef boost::function<void (const ErrorCode&)>         ConnectCallback;
typedef boost::function<void (const ErrorCode&, size_t)> WriteCallback;
typedef boost::function<void (const ErrorCode&, size_t)> ReadCallback;

#endif // __ASIOCOMMON__H__
