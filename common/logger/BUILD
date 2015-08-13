cc_library(
    name = "logger",
    srcs = [
        "Logger.cpp",
    ],
    incs = [
        "../../thirdparty/glog-0.3.3",
        "../../thirdparty/gflag-2.1.1",
        ".",
    ],
    deps = [
        "//thirdparty/glog-0.3.3:glog",
        "//thirdparty/gflag-2.1.1:gflag",
    ],
    defs = [
        "HAVE_LIB_GFLAGS",
    ],
)
