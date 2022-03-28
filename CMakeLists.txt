cmake_minimum_required(VERSION 3.16)
project(CD)

set(CMAKE_CXX_STANDARD 17)


add_subdirectory(libcvd)

add_executable(cvd-fast9 src/cvd_fast9.cpp)
target_link_libraries(cvd-fast9 PUBLIC CVD)
target_include_directories(cvd-fast9 PUBLIC src libcvd)
