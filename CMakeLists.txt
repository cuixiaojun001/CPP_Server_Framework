# CMake 最低版本号要求
cmake_minimum_required(VERSION 2.8)

# 项目信息
project(sylar)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -g -std=c++11 -Wall -fsanitize=undefined -Wno-deprecated -Werror -Wno-unused-function")

include_directories(.)
include_directories(/yaml/yaml-cpp/include)


set(LIB_SRC
    sylar/Log_System/util.cpp
    sylar/Log_System/LogLevel.cpp
    sylar/Log_System/Logger.cpp
    sylar/Log_System/LogEvent.cpp
    sylar/Log_System/LogFormatter.cpp
    sylar/Log_System/LogAppender.cpp
    sylar/Config/config.cpp
    )
add_library(sylar SHARED ${LIB_SRC})

set(LIBS
        sylar
        yaml-cpp
)

# 指定生成目标
# sylar_add_executable(test_config "tests/test_config.cpp" sylar "${LIBS}")
add_executable(test tests/test.cpp)
add_dependencies(test sylar)
target_link_libraries(test sylar)

add_executable(test_config tests/test_config.cpp)
add_dependencies(test_config sylar)
target_link_libraries(test_config sylar yaml-cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

FIND_PACKAGE(Boost)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
ENDIF()
