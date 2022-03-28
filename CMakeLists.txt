cmake_minimum_required(VERSION 3.16)
project(DPHPC)

file(GLOB Sources "src/*.cpp")
message(${Sources})
add_executable(test ${Sources})
target_include_directories(test PUBLIC src)
