cmake_minimum_required(VERSION 3.0.0)
project(bloodmeal VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(bloodmeal src/main.cpp src/humans_at_site.cpp src/construct.cpp)
include_directories(src/)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
