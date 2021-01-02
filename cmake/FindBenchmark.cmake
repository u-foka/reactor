cmake_minimum_required(VERSION 3.1)

if(NOT DEFINED BENCHMARK_INCLUDE_DIR)
   find_path(BENCHMARK_INCLUDE_DIR benchmark/benchmark.h)
endif()
if(NOT DEFINED BENCHMARK_LIBRARY)
   find_library(BENCHMARK_LIBRARY NAMES benchmark)
endif()

if (BENCHMARK_INCLUDE_DIR AND BENCHMARK_LIBRARY)
   set(HAVE_BENCHMARK 1)
endif (BENCHMARK_INCLUDE_DIR AND BENCHMARK_LIBRARY)

if (HAVE_BENCHMARK)
   if (NOT BENCHMARK_FIND_QUIETLY)
      message(STATUS "Found google benchmark: ${BENCHMARK_LIBRARY} include path: ${BENCHMARK_INCLUDE_DIR}")
   endif (NOT BENCHMARK_FIND_QUIETLY)
else (HAVE_BENCHMARK)
   if (BENCHMARK_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find google benchmark")
   endif (BENCHMARK_FIND_REQUIRED)
endif (HAVE_BENCHMARK)
