cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)
set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(Task_9 VERSION ${PROJECT_VERSION})

configure_file(version.h.in version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(headers)

link_directories(/usr/local/lib)
link_directories(${CMAKE_BINARY_DIR})

add_library(async SHARED src/async.cpp)
add_executable(test_lib main.cpp)

target_link_libraries(test_lib PRIVATE stdc++ m async)

set_target_properties(test_lib PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
set_target_properties(async PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_compile_options(test_lib PRIVATE -Wall -Wextra -pedantic)
target_compile_options(async PRIVATE -Wall -Wextra -pedantic)

install(TARGETS test_lib RUNTIME DESTINATION /usr/bin)
install(TARGETS async RUNTIME DESTINATION /usr/local/lib)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_NAME "async")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Vishnenko Maxim")

include(CPack)