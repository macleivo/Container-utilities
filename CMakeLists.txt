cmake_minimum_required(VERSION 3.5)

project(cpp-utilities LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cpp-utilities main.cpp containerutils.h type_traits.h)

install(TARGETS cpp-utilities
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
