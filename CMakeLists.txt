cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# ---- Project ----

# Note: update this to your new project's name and version
project(blpl
  VERSION 0.1.1
  DESCRIPTION "big, lightweight pipeline"
  LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Include guards ----

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

# ---- Add dependencies via CPM ----
# see https://github.com/TheLartians/CPM.cmake for more info

include(cmake/CPM.cmake)

# PackageProject.cmake will be used to make our target installable
CPMAddPackage(
  NAME PackageProject.cmake
  GITHUB_REPOSITORY TheLartians/PackageProject.cmake
  VERSION 1.2
)

# ---- Add source files ----

# Note: globbing sources is considered bad practice as CMake's generators may not detect new files automatically.
# Keep that in mind when changing files, or explicitly mention them here.
FILE(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
FILE(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

# ---- Create library ----

# Note: for header-only libraries change all PUBLIC flags to INTERFACE and create an interface target:
# add_library(blpl INTERFACE)
# set_target_properties(blpl PROPERTIES INTERFACE_COMPILE_FEATURES cxx_std_17)

add_library(blpl ${headers} ${sources})

set_target_properties(blpl PROPERTIES CXX_STANDARD 17)

# beeing a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(blpl PUBLIC "$<$<BOOL:${MSVC}>:/permissive->")

# Link dependencies (if required)
# target_link_libraries(blpl PUBLIC cxxopts)
target_link_libraries(blpl PUBLIC pthread)

target_include_directories(blpl
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)

# ---- Create an installable target ----
# this allows users to install and find the library via `find_package()`.

packageProject(
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
  DEPENDENCIES ""
)
