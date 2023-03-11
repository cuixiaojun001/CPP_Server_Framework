# CMake 最低版本号要求
cmake_minimum_required(VERSION 2.8)

# 项目信息
project(sylar)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -g -std=c++11 -Wall -fsanitize=undefined -Wno-deprecated -Werror -Wno-unused-function")

set(LIB_SRC
    sylar/util.cpp
    sylar/LOG_SYSTEM/LogLevel.cpp
    sylar/LOG_SYSTEM/Logger.cpp
    sylar/LOG_SYSTEM/LogEvent.cpp
    sylar/LOG_SYSTEM/LogFormatter.cpp
    sylar/LOG_SYSTEM/LogAppender.cpp
    sylar/tmp.cpp
    )
add_library(sylar SHARED ${LIB_SRC})

# 指定生成目标
add_executable(test tests/test.cpp)

add_dependencies(test sylar)

target_link_libraries(test sylar)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)