cmake_minimum_required(VERSION 3.16)
project(CD)

set(CMAKE_CXX_STANDARD 17)


add_subdirectory(libcvd)

add_executable(test-fast9 "src/main.cpp")
target_link_libraries(test-fast9 PUBLIC CVD)
target_include_directories(test-fast9 PUBLIC src libcvd)
