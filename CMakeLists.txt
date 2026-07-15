cmake_minimum_required(VERSION 3.16)
project(ExamClock CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(rapidcheck QUIET)
if(NOT rapidcheck_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    rapidcheck
    GIT_REPOSITORY https://github.com/emil-e/rapidcheck.git
    GIT_TAG        b2d9ed2dddefc4b84318d664b4f221eb792d89c7
  )
  FetchContent_MakeAvailable(rapidcheck)
endif()

add_library(clock src/Clock.cpp)
target_include_directories(clock PUBLIC include)

add_executable(all_tests test/all_tests.cpp)
target_link_libraries(all_tests PRIVATE clock rapidcheck)

enable_testing()
add_test(NAME all_tests COMMAND all_tests)
