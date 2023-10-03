cmake_minimum_required(VERSION 3.16)

project(crisp C)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
set_property(TARGET project_options PROPERTY C_STANDARD 11)

# Link this project_warnings 'library' to use the warnings specified in 
# CompilerWarnings.cmake
add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

include(CTest)

add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()