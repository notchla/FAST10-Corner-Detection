cmake_minimum_required(VERSION 3.16)
project(CD)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options(/arch:AVX2)
else()
    add_compile_options(-march=skylake)
endif()


add_subdirectory(libcvd)

set(Sources  "src/fast9_scalar.cpp" "src/fast9_sse2.cpp" "src/fast9_if.cpp" "src/fast10_scalar.cpp" "src/fastX_slow.cpp" "src/fast10_sse2.cpp" "src/fast10_avx2.cpp" "src/utils.cpp")
add_executable(test-fast9 "src/main.cpp" ${Sources})
target_link_libraries(test-fast9 PUBLIC CVD)
target_include_directories(test-fast9 PUBLIC src libcvd)

add_executable(test-vtune "src/test_vtune.cpp" ${Sources})
target_link_libraries(test-vtune PUBLIC CVD)
target_include_directories(test-vtune PUBLIC src libcvd)
target_compile_definitions(test-vtune PUBLIC COUNT_CHECKS=false PEELING_ENABLED=false)


message(${CMAKE_CXX_FLAGS_RELEASE})
