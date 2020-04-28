cmake_minimum_required(VERSION 3.1)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

# GMock macros vs. Clang... :)
enable_cxx_compiler_flag_if_supported("-Wno-gnu-zero-variadic-macro-arguments")

add_subdirectory(unit_tests)
add_subdirectory(reactor_benchmark)

forward_to_parent(FORMAT_FILES)
