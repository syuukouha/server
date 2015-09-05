--[[
FileName : server.lua
Author   : rick <rick.han@yahoo.com>

copyright (c) <2015> <rick>

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
]]--

require 'class'

TcpServer = { 
    _ip          = '127.0.0.1', 
    _port        = 8080,
    _connectFunc = nil, -- 字符串格式的回调函数
    _closeFunc   = nil,
    _msgFunc     = nil,
    _server      = nil,
}

function TcpServer:setConnectFunc(func)
    self._connectFunc = func
end

function TcpServer:setCloseFunc(func)
    self._closeFunc = func
end

function TcpServer:setMsgFunc(func)
    self._msgFunc = func
end

function TcpServer:startListen()
    self._server = Net.NewTcpServer(self._ip, self._port)
    if self._server == nil then
        error('failed to create tcp server')
    end

    local mt = getmetatable(self._server)
    if self._connectFunc ~= nil then
        mt.SetConnectCallback(self._server, self._connectFunc)
        --self._server:SetConnectCallback(self._connectFunc)
    end

    if self._closeFunc ~= nil then
        mt.SetCloseCallback(self._server, self._closeFunc)
        --self._server:SetCloseCallback(self._closeFunc)
    end

    if self._msgFunc ~= nil then
        mt.SetMsgCallback(self._server, self._msgFunc)
        --self._server:SetMsgCallback(self._msgFunc)
    end
end

function TcpServer:close()
    if self._server then
        self._server:Close()
        self._server = nil
    end
end

function createTcpServer(ip, port)
    local s = class('TcpServer', TcpServer)
    s._ip   = ip
    s._port = port

    return s
end

