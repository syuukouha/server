cc_library(
    name = "luaengine",
    srcs = [
        "LuaEngine.cpp",
        "CCLuaValue.cpp"
    ],
    deps = [
        "//thirdparty/lua-5.1:lua",
    ],
    incs = [
        "thirdparty/lua-5.1",
    ],
)

cc_binary(
    name = "testlua",
    srcs = [
        "TestLua.cpp",
    ],
    deps = [
        ":luaengine",
    ],
)
