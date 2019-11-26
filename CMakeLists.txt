cmake_minimum_required(VERSION 3.1)

project(reactor)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
  file (STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" PACKAGED_PRODUCT_VERSION)
  set(PROJECT_VERSION ${PACKAGED_PRODUCT_VERSION})
else()
  execute_process(WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  COMMAND git describe
                  OUTPUT_VARIABLE DETECTED_PRODUCT_VERSION)

  set(PROJECT_VERSION ${DETECTED_PRODUCT_VERSION})
endif()

get_directory_property(HAS_PARENT PARENT_DIRECTORY)
if (HAS_PARENT)
  set(REACTOR_BUILD_MODE "Subproject")
else()
set(REACTOR_BUILD_MODE "Standalone")
endif()

option(REACTOR_STATIC "Build static library" OFF)
if (REACTOR_STATIC)
  set(REACTOR_LIBRARY_TYPE "Static")
else()
  set(REACTOR_LIBRARY_TYPE "Dynamic")
endif()

message("Configuring ${PROJECT_NAME} version ${PROJECT_VERSION} in ${REACTOR_BUILD_MODE} mode as ${CMAKE_BUILD_TYPE} ${REACTOR_LIBRARY_TYPE} library")
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(PROJECT_SOURCE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(HAS_PARENT)
  set(REACTOR_LIBRARY ${PROJECT_NAME} PARENT_SCOPE)
  set(REACTOR_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)
else()
  # Common output directory
  set(COMMON_BUILD_BINARY_DIR ${PROJECT_BINARY_DIR}/bin)
  set(COMMON_BUILD_ARCHIVE_DIR ${PROJECT_BINARY_DIR}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${COMMON_BUILD_BINARY_DIR} CACHE PATH "Single Directory for all Executables.")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${COMMON_BUILD_BINARY_DIR} CACHE PATH "Single Directory for all Libraries")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${COMMON_BUILD_ARCHIVE_DIR} CACHE PATH "Single Directory for all static libraries.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(enable_flag_if_supported)

enable_cxx_compiler_flag_if_supported("-Werror")
enable_cxx_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wextra")
enable_cxx_compiler_flag_if_supported("-pedantic")

include(date_time)
DATE(CURR_DATE)
YEAR(CURR_YEAR)

include_directories("include")

configure_file(src/config.h.in config.h)
add_definitions(-include "\"${CMAKE_CURRENT_BINARY_DIR}/config.h\"")

if(UNIX)
   set(PLATFORM_HEADERS_PATTERN "src/unix/*.hpp")
   set(PLATFORM_SOURCES_PATTERN "src/unix/*.cpp")
endif(UNIX)

file(GLOB SOURCES "src/*.cpp" ${PLATFORM_SOURCES_PATTERN})
file(GLOB HEADERS "src/*.hpp" "include/*.hpp" ${PLATFORM_HEADERS_PATTERN})

include_directories(${ROOT_SOURCE_DIR})

if(REACTOR_STATIC)
  add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
else(REACTOR_STATIC)
  add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
endif(REACTOR_STATIC)

if(NOT HAS_PARENT)
  enable_testing()

  add_subdirectory(tests)

  find_package(Doxygen)
  option(DOXYGEN_ENABLED "enable documentation generation with doxygen" true)
  if(DOXYGEN_FOUND AND DOXYGEN_ENABLED)
      configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
      add_custom_target(doc ALL
          ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile > /dev/null
          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
          COMMENT "Generating API documentation with Doxygen" VERBATIM
      )
  endif(DOXYGEN_FOUND AND DOXYGEN_ENABLED)

  set(CPPCHECK_INCLUDE_ARGS "")
  get_property(CPPCHECK_INCLUDES DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
  foreach(DIR ${CPPCHECK_INCLUDES})
    set(CPPCHECK_INCLUDE_ARGS "${CPPCHECK_INCLUDE_ARGS} -I ${DIR}")
  endforeach()

  add_custom_target(
    cppcheck
    COMMAND cppcheck
    --enable=warning,performance,portability,information,missingInclude
    --std=c++14
    --template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)"
    --verbose
    --quiet
    ${CPPCHECK_INCLUDE_ARGS}
    ${SOURCES}
  )

  # Adding clang-format target if executable is found
  find_program(CLANG_FORMAT "clang-format")
  if(CLANG_FORMAT)
    add_custom_target(
      clang-format
      COMMAND ${CLANG_FORMAT}
      -i
      -style=file
      ${SOURCES} ${HEADERS}
      )
  endif()

  # Adding clang-tidy target if executable is found
  find_program(CLANG_TIDY "clang-tidy")
  if(CLANG_TIDY)
    add_custom_target(
      clang-tidy
      COMMAND ${CLANG_TIDY}
      ${SOURCES} ${HEADERS}
      -config=''
      --
      -std=c++17
      ${INCLUDE_DIRECTORIES}
      )
  endif()
endif(NOT HAS_PARENT)
