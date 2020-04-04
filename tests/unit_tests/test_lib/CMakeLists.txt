cmake_minimum_required(VERSION 3.1)

file(GLOB TEST_SOURCES *.cpp)
file(GLOB TEST_HEADERS *.hpp)
list(APPEND FORMAT_FILES ${TEST_SOURCES} ${TEST_HEADERS})

forward_to_parent(FORMAT_FILES)

include_directories(${PROJECT_SOURCE_ROOT_DIR})

# Using cmake objlib feature instead of static lib as it only links referenced symbols,
# so our factories wouldn't register.. there are different whole-link switches for 
# different toolchains but there is no simple way to do that :(
add_library(test_lib OBJECT ${TEST_SOURCES})
