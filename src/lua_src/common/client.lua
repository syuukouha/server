--[[
FileName : client.lua
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

TcpClient = { 
    _ip          = '127.0.0.1', 
    _port        = 8080,
    _connectFunc = nil, -- 字符串格式的回调函数
    _closeFunc   = nil,
    _msgFunc     = nil,
    _client      = nil,
}

function TcpClient:setConnectFunc(func)
    self._connectFunc = func
end

function TcpClient:setCloseFunc(func)
    self._closeFunc = func
end

function TcpClient:setMsgFunc(func)
    self._msgFunc = func
end

function TcpClient:printall()
    local str = 'ip=' .. self._ip .. ' port=' .. self._port
    str = str .. tostring(self._connectFunc)
    print(str)
end

function TcpClient:startConnect()
    self._client = Net.NewTcpClient(self._ip, self._port)
    if self._client == nil then
        error('failed to create tcp client')
    end
   
    local mt = getmetatable(self._client)
    if mt == nil then
        error('no metatable found!')
    end
    print (self._ip .. " : " .. self._port)
    if self._connectFunc ~= nil then
        mt.SetConnectCallback(self._client, self._connectFunc)
        --self._client:SetConnectCallback(self._connectFunc)
    end

    if self._closeFunc ~= nil then
        mt.SetCloseCallback(self._client, self._closeFunc)
        --self._client:SetCloseCallback(self._closeFunc)
    end

    if self._msgFunc ~= nil then
        mt.SetMsgCallback(self._client, self._msgFunc)
        --self._client:SetMsgCallback(self._msgFunc)
    end

    mt.Connect(self._client)
end

function TcpClient:close()
    if self._client then
        self._client:Close()
        self._client = nil
    end
end

function createTcpClient(ip, port)
    local c = class("TcpClient", TcpClient)
    c._ip   = ip
    c._port = port

    return c
end
