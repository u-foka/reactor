cmake_minimum_required(VERSION 3.1)

project(reactor)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
  # Parse version information delivered with the package
  file (STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" PACKAGED_PRODUCT_VERSION)
  set(PRODUCT_VERSION PACKAGED_PRODUCT_VERSION)
  list(GET PACKAGED_PRODUCT_VERSION 0 PROJECT_VERSION)
  list(GET PACKAGED_PRODUCT_VERSION 1 PROJECT_SOVERSION)
else()
  # Detect version from git tag
  execute_process(WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  COMMAND git describe --all --match "v*"
                  OUTPUT_VARIABLE PROJECT_VERSION)
  string(REGEX REPLACE "^tags/v(.+)\n$" "\\1" PROJECT_VERSION "${PROJECT_VERSION}")
  string(REGEX REPLACE "^([(0-9)]+\\.[(0-9)]+).*$" "\\1" PROJECT_SOVERSION "${PROJECT_VERSION}")
  set(DETECTED_PRODUCT_VERSION ${PROJECT_VERSION} ${PROJECT_SOVERSION})
  set(PRODUCT_VERSION ${DETECTED_PRODUCT_VERSION})
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

message("Configuring ${PROJECT_NAME} version ${PRODUCT_VERSION} in ${REACTOR_BUILD_MODE} mode as ${CMAKE_BUILD_TYPE} ${REACTOR_LIBRARY_TYPE} library")
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(PROJECT_SOURCE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(HAS_PARENT)
  # When building as a subproject, skip common location setup and pass library variables up
  
  set(REACTOR_LIBRARY ${PROJECT_NAME} PARENT_SCOPE)
  set(REACTOR_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)
else()
  # Setup standalone build

  set(REACTOR_LIBRARY ${PROJECT_NAME})
  set(REACTOR_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

  # Common output directory
  set(COMMON_BUILD_BINARY_DIR ${PROJECT_BINARY_DIR}/bin)
  set(COMMON_BUILD_ARCHIVE_DIR ${PROJECT_BINARY_DIR}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${COMMON_BUILD_BINARY_DIR} CACHE PATH "Single Directory for all Executables.")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${COMMON_BUILD_BINARY_DIR} CACHE PATH "Single Directory for all Libraries")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${COMMON_BUILD_ARCHIVE_DIR} CACHE PATH "Single Directory for all static libraries.")

  # Setup googletest up here so it wont get our warning flags
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # Build googletest with shared runtime on windows
  add_subdirectory(thirdparty/googletest)
endif()

option(REACTOR_CXX11_ENABLED "restrict c++ standard to c++11 (instead of the default c++14)" false)
option(REACTOR_CXX17_ENABLED "restrict c++ standard to c++17 (instead of the default c++14, REACTOR_CXX11_ENABLED overrides this)" false)

if(REACTOR_CXX11_ENABLED)
    message("Configuring reactor with C++11 standard")
    set(CMAKE_CXX_STANDARD 11)
elseif(REACTOR_CXX17_ENABLED)
    message("Configuring reactor with C++17 standard")
    set(CMAKE_CXX_STANDARD 17)
else()
    message("Configuring reactor with C++14 standard")
    set(CMAKE_CXX_STANDARD 14)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(MSVC)
    add_compile_options("/Zc:__cplusplus")
endif(MSVC)

include(enable_flag_if_supported)

enable_cxx_compiler_flag_if_supported("-Werror")
enable_cxx_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wextra")
enable_cxx_compiler_flag_if_supported("-pedantic")

include(date_time)
DATE(CURR_DATE)
YEAR(CURR_YEAR)

include(forward_to_parent)

include_directories("include")

configure_file(src/config.h.in config.h)
if(MSVC)
  add_definitions(/FI "\"${CMAKE_CURRENT_BINARY_DIR}/config.h\"")
else(MSVC)
  add_definitions(-include "\"${CMAKE_CURRENT_BINARY_DIR}/config.h\"")
endif(MSVC)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.hpp" "include/reactor/*.hpp")

set(FORMAT_FILES ${SOURCES} ${HEADERS})

include_directories(${ROOT_SOURCE_DIR})

if(REACTOR_STATIC)
  add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
else(REACTOR_STATIC)
  # To build shared libraries in Windows, we set CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS to TRUE.
  # See https://cmake.org/cmake/help/v3.4/variable/CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS.html
  # See https://blog.kitware.com/create-dlls-on-windows-without-declspec-using-new-cmake-export-all-feature/
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

  add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

  # This will name the output .so files "libsomething.1.0" which is pretty useful
  set_target_properties(${PROJECT_NAME} PROPERTIES 
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_SOVERSION})
endif(REACTOR_STATIC)

target_compile_definitions(${PROJECT_NAME} PRIVATE REACTOR_LIBRARY)

if(NOT HAS_PARENT)
  # Version file generation for packaging
  option(GENERATE_VERSION "generate VERSION file in the source root directory for packaging" false)
  if(GENERATE_VERSION)
    configure_file(src/VERSION.in ${CMAKE_CURRENT_SOURCE_DIR}/VERSION)
  endif(GENERATE_VERSION)

  # Packaging support for standalone mode
  set(CPACK_SOURCE_GENERATOR "ZIP")
  set(CPACK_SOURCE_IGNORE_FILES
    /.git
    /.vs
    /.vscode
    /CMakeSettings.json
    /out
    /thirdparty
    /.*build.*
    /\\\\.DS_Store
  )
  include(CPack)

  # In standalone mode setup the testing and maintenance targets
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
    list(REMOVE_DUPLICATES FORMAT_FILES)
    add_custom_target(
      clang-format
      COMMAND ${CLANG_FORMAT}
      -i
      -style=file
      ${FORMAT_FILES}
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
