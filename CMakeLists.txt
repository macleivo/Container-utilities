cmake_minimum_required(VERSION 3.5)

project(cpp-utiliies LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cpp-utiliies main.cpp containerutils.h type_traits.h enumerate.h)

install(TARGETS cpp-utiliies
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
