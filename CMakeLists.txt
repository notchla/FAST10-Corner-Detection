cmake_minimum_required(VERSION 3.16)
project(CD)

set(CMAKE_CXX_STANDARD 17)

set(AVX512 TRUE)

if(AVX512)
    add_compile_definitions(AVX512_ENABLED)
endif()

if(MSVC)
    if(AVX512)
        add_compile_options(/arch:AVX512)
    else()
        add_compile_options(/arch:AVX2)
    endif() 
else()
    add_compile_options(-march=native)
endif()


add_subdirectory(libcvd)

file(GLOB Sources "src/fast*.cpp" "src/utils.cpp")
add_executable(test-fast9 "src/main.cpp" ${Sources})
target_link_libraries(test-fast9 PUBLIC CVD)
target_include_directories(test-fast9 PUBLIC src libcvd)

add_executable(test-vtune "src/test_vtune.cpp" ${Sources})
target_link_libraries(test-vtune PUBLIC CVD)
target_include_directories(test-vtune PUBLIC src libcvd)
target_compile_definitions(test-vtune PUBLIC COUNT_CHECKS=false PEELING_ENABLED=false)


message("release flags" ${CMAKE_CXX_FLAGS_RELEASE})
