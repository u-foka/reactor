cmake_minimum_required(VERSION 3.1)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include(code-coverage)
add_code_coverage_all_targets()

file(GLOB_RECURSE TEST_SOURCES *.cpp)
file(GLOB_RECURSE TEST_HEADERS *.hpp)

file(GLOB_RECURSE MOCK_SOURCES ${ROOT_SOURCE_DIR}/mocks/*.cpp)
file(GLOB_RECURSE MOCK_HEADERS ${ROOT_SOURCE_DIR}/mocks/*.hpp)

include_directories("../../thirdparty/gtest/include")
include_directories("../../thirdparty/gmock/include")

include_directories("../../thirdparty/gtest")
include_directories("../../thirdparty/gmock")

include_directories("../../src")

foreach(item ${SOURCES})
  if(NOT item MATCHES "main.cpp$")
    list(APPEND TEST_SOURCES ${item})
  endif()
endforeach()

set(TEST_NAME ${PROJECT_NAME}_test)

add_executable(${TEST_NAME} ${TEST_SOURCES} ${TEST_HEADERS} ${MOCK_SOURCES} ${MOCK_HEADERS} "../../thirdparty/gtest/src/gtest-all.cc" "../../thirdparty/gmock/src/gmock-all.cc" "../../thirdparty/gmock/src/gmock_main.cc")
target_link_libraries(${TEST_NAME} Threads::Threads)
target_code_coverage(${TEST_NAME} AUTO ALL EXCLUDE tests/* thirdparty/*)

add_test(${TEST_NAME} ${COMMON_BUILD_BINARY_DIR}/${TEST_NAME})
add_dependencies(check ${TEST_NAME})
