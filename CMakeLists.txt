cmake_minimum_required(VERSION 3.16)
project(CD)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options(/arch:AVX2)
else()
    add_compile_options(-march=skylake)
endif()


add_subdirectory(libcvd)

add_executable(test-fast9 "src/main.cpp" "src/fast9_scalar.cpp" "src/fast9_sse2.cpp" "src/fast.h" "src/fast9_if.cpp" "src/fast10_scalar.cpp" "src/fastX_slow.cpp" "src/fast10_sse2.cpp" "src/fast10_avx2.cpp")
target_link_libraries(test-fast9 PUBLIC CVD)
target_include_directories(test-fast9 PUBLIC src libcvd)


message(${CMAKE_CXX_FLAGS_RELEASE})
