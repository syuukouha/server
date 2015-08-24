
function CalledByCpp()
    print "NoArg func called by cpp"
    local dir = lfs.currentdir()
    print ('curr_dir=' .. dir)
    for file in lfs.dir(dir) do
        print (file)
    end

    local tcpClient = Net.NewTcpClient('127.0.0.1', 80)
    if tcpClient == nil then
        print ('nilnilnil')
    else
        tcpClient:connect()
    end
end

function TwoArgs(arg1, arg2)
    print ("arg1= " .. arg1 .. " arg2=" .. arg2)
    local r = Test.CppFunc(123,true,"func")
    print (r)
end

function TwoArgsAndRets(arg1, arg2)
    print ("arg1= " .. arg1 .. " arg2=" .. arg2)
    return 1, "test"
end


print "main.lua is loaded by cpp"
