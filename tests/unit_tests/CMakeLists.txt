cmake_minimum_required(VERSION 3.1)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include(code-coverage)
add_code_coverage_all_targets()

file(GLOB_RECURSE TEST_SOURCES *.cpp)
file(GLOB_RECURSE TEST_HEADERS *.hpp)
list(APPEND FORMAT_FILES ${TEST_SOURCES} ${TEST_HEADERS})

file(GLOB_RECURSE MOCK_SOURCES ${ROOT_SOURCE_DIR}/mocks/*.cpp)
file(GLOB_RECURSE MOCK_HEADERS ${ROOT_SOURCE_DIR}/mocks/*.hpp)
list(APPEND FORMAT_FILES ${MOCK_SOURCES} ${MOCK_HEADERS})

forward_to_parent(FORMAT_FILES)

set(GTEST_ROOT "../../thirdparty/googletest/googletest")
set(GMOCK_ROOT "../../thirdparty/googletest/googlemock")

include_directories("${GTEST_ROOT}/include")
include_directories("${GMOCK_ROOT}/include")

include_directories("${GTEST_ROOT}")
include_directories("${GMOCK_ROOT}")

include_directories("../../src")

set(TEST_NAME ${PROJECT_NAME}_test)

add_executable(${TEST_NAME} ${TEST_SOURCES} ${TEST_HEADERS} ${MOCK_SOURCES} ${MOCK_HEADERS} ${SOURCES} "${GTEST_ROOT}/src/gtest-all.cc" "${GMOCK_ROOT}/src/gmock-all.cc" "${GMOCK_ROOT}/src/gmock_main.cc")
target_link_libraries(${TEST_NAME} PRIVATE Threads::Threads)
target_code_coverage(${TEST_NAME} AUTO ALL EXCLUDE tests/* thirdparty/*)

add_test(${TEST_NAME} ${COMMON_BUILD_BINARY_DIR}/${TEST_NAME})
add_dependencies(check ${TEST_NAME})
