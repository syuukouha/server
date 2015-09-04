cc_library(
    name = "luaengine",
    srcs = [
        "CCLuaValue.cpp",
        "LuaEngine.cpp",
        'lfs.c',
        'LuaNetModule.cpp',
        'LuaSysModule.cpp',
    ],
    deps = [
        "//thirdparty/lua-5.1:lua",
        "//common/logger:logger",
    ],
    incs = [
        "../../thirdparty/lua-5.1",
        "../logger",
        "../../thirdparty/glog-0.3.3",
        "../network",
    ],
)

cc_binary(
    name = "testlua",
    srcs = ["TestLua.cpp"],
    deps = [
        ":luaengine", 
        "#pthread",
        "#boost_system",
        '//common/network:network'
    ],
    incs = ["../../thirdparty/lua-5.1", "../logger", "../../thirdparty/glog-0.3.3"],
)

cc_binary(
    name = "ELua",
    srcs = ["ELua.cpp"],
    deps = [
        ":luaengine",
        "#pthread",
        "#boost_system",
        "//common/network:network",
    ],
    incs = ["../../thirdparty/lua-5.1", "../logger", "../../thirdparty/glog-0.3.3"],
)
