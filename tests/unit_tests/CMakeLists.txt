cmake_minimum_required(VERSION 3.5)

add_subdirectory("test_lib")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

file(GLOB TEST_SOURCES *.cpp)
file(GLOB TEST_HEADERS *.hpp)
list(APPEND FORMAT_FILES ${TEST_SOURCES} ${TEST_HEADERS})

forward_to_parent(FORMAT_FILES)

include_directories(${PROJECT_SOURCE_ROOT_DIR})

set(TEST_NAME ${PROJECT_NAME}_test)

add_executable(${TEST_NAME} ${TEST_SOURCES} ${TEST_HEADERS} ${MOCK_SOURCES} ${MOCK_HEADERS})
target_sources(${TEST_NAME} PRIVATE $<TARGET_OBJECTS:test_lib>)
if(REACTOR_SHARED)
	target_compile_definitions(${TEST_NAME} PRIVATE REACTOR_SHARED)
endif(REACTOR_SHARED)
target_link_libraries(${TEST_NAME} PRIVATE ${REACTOR_LIBRARY} gmock gtest Threads::Threads)
target_code_coverage(${TEST_NAME} AUTO ALL EXCLUDE tests/* thirdparty/*)

add_test(${TEST_NAME} ${COMMON_BUILD_BINARY_DIR}/${TEST_NAME})
add_dependencies(check ${TEST_NAME})
