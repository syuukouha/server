
function CalledByCpp()
    print "NoArg func called by cpp"
end

function TwoArgs(arg1, arg2)
    print ("arg1= " .. arg1 .. " arg2=" .. arg2)
end

function TwoArgsAndRets(arg1, arg2)
    print ("arg1= " .. arg1 .. " arg2=" .. arg2)
    return 1, "test"
end

print "main.lua is loaded by cpp"
