# generated by genlibbuild

cc_library(
    name = 'gtest',
    srcs = [
        'gtest-all.cc',
        'gtest.cc',
        'gtest-death-test.cc',
        'gtest-filepath.cc',
        'gtest-port.cc',
        'gtest-printers.cc',
        'gtest-test-part.cc',
        'gtest-typed-test.cc',
    ],
    incs = [
        ".",
    ],
)

cc_binary(
    name = "gtest_main",
    srcs = [
        'gtest_main.cc',
    ],
    deps = [
        ':gtest',
        "#pthread",
    ],
    incs = [
        ".",
    ],
)
