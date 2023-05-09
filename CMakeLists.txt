cmake_minimum_required(VERSION 2.8)
project(sylar)

include (cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

include_directories(.)
include_directories(/apps/sylar/include)
link_directories(/apps/sylar/lib)

find_library(YAMLCPP yaml-cpp)
# message("***", ${YAMLCPP})

set(LIB_SRC
    sylar/log.cc
    sylar/util.cc
    sylar/config.cc
    sylar/thread.cpp
    sylar/mutex.cpp
    sylar/fiber.cpp
    sylar/scheduler.cpp
    sylar/iomanager.cpp
    sylar/timer.cpp
    sylar/fd_manager.cpp
    sylar/hook.cpp
    )

add_library(sylar SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(sylar) #__FILE__
#add_library(sylar_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERTIES (sylar_static PROPERTIES OUTPUT_NAME "sylar")

set(LIB_LIB
    sylar
    dl
    pthread
    ${YAMLCPP}
)

# add_executable(test tests/test.cc)
# add_dependencies(test sylar)
# force_redefine_file_macro_for_sources(test) #__FILE__
# target_link_libraries(test sylar ${LIB_LIB})

# add_executable(test_config tests/test_config.cc)
# add_dependencies(test_config sylar)
# force_redefine_file_macro_for_sources(test_config) #__FILE__
# target_link_libraries(test_config sylar ${LIB_LIB})

# add_executable(test_thread tests/test_thread.cpp)
# add_dependencies(test_thread sylar)
# force_redefine_file_macro_for_sources(test_thread) #__FILE__
# target_link_libraries(test_thread sylar ${LIB_LIB})

# add_executable(test_atomic tests/test_atomic.cpp)
# add_dependencies(test_atomic sylar)
# force_redefine_file_macro_for_sources(test_atomic) #__FILE__
# target_link_libraries(test_atomic sylar ${LIB_LIB})

# add_executable(test_util tests/test_util.cpp)
# add_dependencies(test_util sylar)
# force_redefine_file_macro_for_sources(test_util) #__FILE__
# target_link_libraries(test_util sylar ${LIB_LIB})

# add_executable(test_fiber tests/test_fiber.cpp)
# add_dependencies(test_fiber sylar)
# force_redefine_file_macro_for_sources(test_fiber) #__FILE__
# target_link_libraries(test_fiber sylar ${LIB_LIB})

add_executable(test_scheduler tests/test_scheduler.cpp)
add_dependencies(test_scheduler sylar)
force_redefine_file_macro_for_sources(test_scheduler) #__FILE__
target_link_libraries(test_scheduler sylar ${LIB_LIB})

add_executable(test_iomanager tests/test_iomanager.cpp)
add_dependencies(test_iomanager sylar)
force_redefine_file_macro_for_sources(test_iomanager) #__FILE__
target_link_libraries(test_iomanager sylar ${LIB_LIB})

add_executable(test_hook tests/test_hook.cpp)
add_dependencies(test_hook sylar)
force_redefine_file_macro_for_sources(test_hook) #__FILE__
target_link_libraries(test_hook sylar ${LIB_LIB})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
