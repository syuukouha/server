/**
main.cpp
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
#include "main.h"
#include "AsioCommon.h"
#include "TcpServer.h"
#include "Logger.h"
#include <string>
#include <iostream>
#include <boost/bind.hpp>
using namespace std;

void handleClose(const ErrorCode& err, const TcpConnPtr& conn)
{
  LOG(ERROR) << "Connection destroyed!";
}

void handleRead(const ErrorCode& err, size_t bytes) 
{
  LOG(ERROR) << "read " << bytes << " data";
} 

void handleWrite(const ErrorCode& err, size_t bytes)
{
  LOG(ERROR) << "write " << bytes << " data";
}

bool handleParse(const TcpConnPtr& conn, const char* data, size_t sz)
{
  if (sz == 0) {
    LOG(ERROR) << "failed to parse data";
    return false;
  }

  // print every byte
  std::string repy = ""; // 测试这样写而已
  for (size_t i=0; i<sz; ++i) {
    LOG(ERROR) << "data[" << i << "] = " << data[i];
    repy += data[i];
  }
  conn->asyncWrite(repy);
  return true;
}

void handleNewConn(const ErrorCode& err, const TcpConnPtr& conn) 
{
  LOG(ERROR) << "New Connection Comes in!!!";
  conn->setCloseCallback(boost::bind(handleClose, _1, _2));
  conn->setWriteCallback(boost::bind(handleWrite, _1, _2));
  //conn->setReadCallback(boost::bind(handleRead, _1, _2));
  conn->setParseCallback(boost::bind(handleParse, _1, _2, _3));
}


int main(int argc, char** argv) 
{
  Logger::init(&argc, argv);
  LOG(ERROR) << "TEST LOG";
  IoService ioService;
  string ip = "0.0.0.0";
  TcpServer tcpServer(ioService, ip, 9999, boost::bind(handleNewConn, _1, _2));
  ioService.run();
  return 0;
}
