cmake_minimum_required(VERSION  3.8)
project(UniversalThread)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

if(CMAKE_SYSTEM_NAME MATCHES "^Linux")
    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")

    set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -std=c++11  -Wall -w -O0 -fPIC -pthread -fsigned-char -save-temps")
    set(CMAKE_CXX_FLAGS_RELEASE "-std=c++11  -Wall -w -O3 -fPIC -pthread -fsigned-char")
    set(CMAKE_C_FLAGS_DEBUG "-g -ggdb -std=c11 -Wall -w -O0 -fPIC -pthread -fsigned-char -save-temps")
    set(CMAKE_C_FLAGS_RELEASE "-std=c11 -Wall -w -O3 -fPIC -pthread -fsigned-char")

    include_directories(${CMAKE_SOURCE_DIR})
    file(GLOB Thread_SRCS ${CMAKE_SOURCE_DIR}/source/linux/uthread.c)
    message("Thread_SRCS: " ${Thread_SRCS})
    add_library(UThread SHARED ${Thread_SRCS})

    set(Thread_DEPS "")
    list(APPEND Thread_DEPS UThread)
    set(Thread_DEPS -Wl,--whole-archive ${Thread_DEPS} -lpthread -Wl,--no-whole-archive)

    add_executable(demo_simple.out ${CMAKE_CURRENT_LIST_DIR}/demo/demo_simple.c)
    target_link_libraries(demo_simple.out ${Thread_DEPS})
elseif((CMAKE_SYSTEM_NAME MATCHES "^Windows"))
    if(MSVC)
        add_definitions(-DBUILDING_DLL)
        include_directories(${CMAKE_SOURCE_DIR})
        file(GLOB Thread_SRCS ${CMAKE_SOURCE_DIR}/source/windows/uthread.c)
        message("Thread_SRCS: " ${Thread_SRCS})
        add_library(UThread SHARED ${Thread_SRCS})

        set(Thread_DEPS "")
        list(APPEND Thread_DEPS UThread)

        add_executable(demo_simple ${CMAKE_CURRENT_LIST_DIR}/demo/demo_simple.c)
        target_link_libraries(demo_simple ${Thread_DEPS})
    endif()
else()
    message(WARNING "Unsupported platform!")
endif()


