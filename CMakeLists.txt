cmake_minimum_required(VERSION 3.5)

project(ContainerUtilities LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ContainerUtilities main.cpp containerutils.h containerutils.cpp type_traits.h)

install(TARGETS ContainerUtilities
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
