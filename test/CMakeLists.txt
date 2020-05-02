cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(blplTests
  LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Options ----

option(ENABLE_TEST_COVERAGE "Enable test coverage" OFF)
option(TEST_INSTALLED_VERSION "Test the version found by find_package" OFF)

# --- Import tools ----

include(../cmake/tools.cmake)

# ---- Dependencies ----

include(../cmake/CPM.cmake)

CPMAddPackage(
  NAME doctest
  GITHUB_REPOSITORY onqtam/doctest
  GIT_TAG 2.3.7
)

if (TEST_INSTALLED_VERSION)
  find_package(blpl REQUIRED)
else()
  CPMAddPackage(
    NAME blpl
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..
  )
endif()

CPMAddPackage(
  NAME Format.cmake
  GITHUB_REPOSITORY TheLartians/Format.cmake
  VERSION 1.3
)

# ---- Create binary ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
add_executable(blplTests ${sources})
target_link_libraries(blplTests doctest blpl)

set_target_properties(blplTests PROPERTIES CXX_STANDARD 17)

# enable compiler warnings
if (NOT TEST_INSTALLED_VERSION)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(blpl PUBLIC -Wall -pedantic -Wextra -Werror)
  elseif(MSVC)
    target_compile_options(blpl PUBLIC /W4 /WX)
    target_compile_definitions(blplTests PUBLIC DOCTEST_CONFIG_USE_STD_HEADERS)
  endif()
endif()

# ---- Add blplTests ----

ENABLE_TESTING() 

# Note: doctest and similar testing frameworks can automatically configure CMake tests
# For other testing frameworks add the tests target instead:
# ADD_TEST(blplTests blplTests)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(blplTests)

# ---- code coverage ----

if (ENABLE_TEST_COVERAGE)
  target_compile_options(blpl PUBLIC -O0 -g -fprofile-arcs -ftest-coverage)
  target_link_options(blpl PUBLIC -fprofile-arcs -ftest-coverage)
endif()
