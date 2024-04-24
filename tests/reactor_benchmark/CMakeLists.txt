cmake_minimum_required(VERSION 3.5)

find_package(Benchmark)

if (BENCHMARK_LIBRARY)
  file(GLOB_RECURSE TEST_SOURCES *.cpp)
  file(GLOB_RECURSE TEST_HEADERS *.hpp)
  list(APPEND FORMAT_FILES ${TEST_SOURCES} ${TEST_HEADERS})
  forward_to_parent(FORMAT_FILES)

  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)

  include_directories("${BENCHMARK_INCLUDE_DIR}" "../../src")

  set(TEST_NAME ${PROJECT_NAME}_benchmark)

  add_executable(${TEST_NAME} ${TEST_SOURCES} ${TEST_HEADERS} ${SOURCES})
  target_link_libraries(${TEST_NAME} PRIVATE Threads::Threads ${BENCHMARK_LIBRARY})
else (BENCHMARK_LIBRARY)
  message("Could not find libbenchmark, skipping benchmark project")
endif(BENCHMARK_LIBRARY)