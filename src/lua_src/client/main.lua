--[[
FileName : main.lua
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

module('TestClient', package.seeall)

clientData = {}

function onConnected(connected, connId)
    if connected then
        print('connected!!')
        clientData.connId = connId
    else
        print('failed to connect')
    end
end

function onMsg(connId, method, data)
    print ('method:' .. method .. " data:" .. data)
end

function onClose(connId)
    if clientData.connId == connId then
        print('connection lost!!!')
        clientData.connId = nil
    end
end

local client = createTcpClient('127.0.0.1', 8080)
client:setConnectFunc("TestClient.onConnected")
client:setMsgFunc('TestClient.onMsg')
client:setCloseFunc('TestClient.onClose')

clientData.client = client
safeCall(client.startConnect, client)
--client:startConnect()

Net.Start()
