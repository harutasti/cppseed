cmake_minimum_required(VERSION 3.20)
project(cppseed VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(CPPSEED_BUILD_TESTS_DEFAULT ON)
else()
  set(CPPSEED_BUILD_TESTS_DEFAULT OFF)
endif()

option(CPPSEED_BUILD_TESTS "Build cppseed tests" ${CPPSEED_BUILD_TESTS_DEFAULT})
option(CPPSEED_WARNINGS_AS_ERRORS "Treat cppseed warnings as errors" OFF)

include(cmake/CompilerWarnings.cmake)

add_library(
  cppseed_core STATIC
  src/application.cpp
  src/cli.cpp
  src/diagnostic.cpp
  src/file_system.cpp
  src/project_generator.cpp
  src/project_name.cpp
  src/template_catalog.cpp
  src/template_renderer.cpp
  src/types.cpp
)
target_include_directories(cppseed_core PUBLIC include)
target_compile_features(cppseed_core PUBLIC cxx_std_20)
cppseed_set_warnings(cppseed_core)

add_executable(cppseed src/main.cpp)
target_link_libraries(cppseed PRIVATE cppseed_core)
target_compile_features(cppseed PRIVATE cxx_std_20)
cppseed_set_warnings(cppseed)

if(CPPSEED_BUILD_TESTS)
  include(CTest)
  add_subdirectory(tests)
endif()
