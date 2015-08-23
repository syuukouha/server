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
#include "common/network/TcpClient.h"
#include "common/network/AsioCommon.h"
#include <string>
#include <iostream>
#include <boost/bind.hpp>
#include "common/network/TcpConnection.h"
#include <unistd.h>
void onConnect(const ErrorCode& err, const TcpConnPtr& conn)
{
  if (!err) {
    std::cout << "连接建立成功！！！！！" << std::endl;
    std::string sendStr = "testsending!";
    for (int i=1; i<2; ++i) {
      conn->asyncWrite(sendStr);
      //sleep(1);
    }
  } else {
    std::cout << "连接建立失败！！！！！自动重连" << std::endl;
  }
}

void handleWrite(const ErrorCode& err, size_t bytes)
{
  if (err) {
    std::cout << "发送数据失败";
    return;
  }

  std::cout << "发送成功，大小=" << bytes << "字节" << std::endl;
}

bool handleParse(const TcpConnPtr& conn, const char* data, size_t sz)
{
  std::cout << "接收到数据 size=" << sz << "字节" << std::endl;
  for (size_t s=0; s<sz; ++s) {
    std::cout << data[s];
  }
  std::cout << std::endl;
  std::string sendStr = "testsending!";
  conn->asyncWrite(sendStr);
  return true;
}

int main(int argc, char** argv)
{
  IoService ioService;
  std::string ip = "127.0.0.1";
  TcpClient client(ioService, ip, 9999,
                   boost::bind(onConnect, _1, _2));
  client.setParseCallback(boost::bind(handleParse, _1, _2, _3));
  client.setWriteCallback(boost::bind(handleWrite, _1, _2));
  client.connect();
  ioService.run();
  return 0;
}
