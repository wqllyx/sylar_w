cmake_minimum_required(VERSION 3.15)
project(sylar)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")
set(LIB_SRC
    src/log.cpp
    src/util.cpp
    )
add_library(sylar SHARED ${LIB_SRC})

add_executable(test tests/test.cpp)
add_dependencies(test sylar)
target_link_libraries(test sylar)
target_include_directories(test PUBLIC ${PROJECT_SOURCE_DIR}/src)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)