cmake_minimum_required(VERSION 3.5)

project(cpp-utilities LANGUAGES CXX)

find_package(Catch2 3 REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(. tests)

add_executable(cpp-utilities tests/tests_container_utils.cpp tests/tests_pipe.cpp containerutils.h type_traits.h pipes.h tests/helpers.h)
target_link_libraries(cpp-utilities PRIVATE Catch2::Catch2WithMain)

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined")

install(TARGETS cpp-utilities
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
