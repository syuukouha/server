cc_library(
    name = "luaengine",
    srcs = [
        "CCLuaValue.cpp",
        "LuaEngine.cpp",
        'lfs.c',
        'LuaNetModule.cpp',
    ],
    deps = [
        "//thirdparty/lua-5.1:lua",
        "//common/logger:logger",
    ],
    incs = [
        "../../thirdparty/lua-5.1",
        "../logger",
        "../../thirdparty/glog-0.3.3",
    ],
)

cc_binary(
    name = "testlua",
    srcs = ["TestLua.cpp"],
    deps = [":luaengine", "#pthread"],
    incs = ["../../thirdparty/lua-5.1", "../logger", "../../thirdparty/glog-0.3.3"],
)
