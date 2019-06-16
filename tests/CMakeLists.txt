cmake_minimum_required(VERSION 3.1)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

add_subdirectory(unit_tests)
add_subdirectory(reactor_benchmark)
