#pragma once

#include <cvd/image_io.h>
#include <cvd/fast_corner.h>
#include <cvd_src/faster_corner_utilities.h>

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <sstream>

#ifdef WIN32
#include <intrin.h>
#include <immintrin.h>

#else
#include <x86intrin.h>
#endif

#ifndef COUNT_CHECKS
#define COUNT_CHECKS true
#endif

#if COUNT_CHECKS
extern uint64_t check[9];
#endif


#ifndef PEELING_ENABLED
#define PEELING_ENABLED true
#endif


#define CHECK_BARRIER_SCALAR(lo, hi, other, flags)			\
{													    	\
    flags = (lo > other) | ((other > hi) << 1);               \
}

using CVD::ImageRef;

inline uint64_t rdtsc() {
    return __rdtsc();
}

namespace CVD {
    void fast_corner_detect_plain_9(const BasicImage<byte>& i, std::vector<ImageRef>& corners, int b);
#include <cvd_src/corner_9.h>
#include <cvd_src/corner_10.h>
}

typedef void fast_func(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);
typedef void cvd_fast_func(const CVD::BasicImage<CVD::byte>& i, std::vector<ImageRef>& corners, int b);

void fast9_sse2   (uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);
void fast9_scalar (uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);
void fast9_if     (uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);

void fast10_scalar(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);
void fast10_sse2  (uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);
void fast10_avx2  (uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);
void fast10_avx512  (uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);
void fastX_slow   (uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);
