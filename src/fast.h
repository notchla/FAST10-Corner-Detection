#pragma once

#define _CRT_SECURE_NO_WARNINGS

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
#define COUNT_CHECKS false
#endif

#if COUNT_CHECKS
extern uint64_t check[10];
#endif

#ifndef COUNT_CHECK_POSITION
#define COUNT_CHECK_POSITION false
#endif

struct Lane {
    int x, y, width, checks;
};
#if COUNT_CHECK_POSITION
extern std::vector<Lane> lane_checks;
extern uint8_t* lane_check;
#endif


#ifndef PEELING_ENABLED
#define PEELING_ENABLED false
#endif

#ifndef WARM_CACHE
#define WARM_CACHE true
#endif

#ifndef TRAIN_BP
#define TRAIN_BP false
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

void fast10_avx2_unrolled_2(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);
void fast10_avx2_unrolled_3(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);

void fast10_scalar_block(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);
//void fast10_avx2_blocking(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);
void fast10_avx2_blocking_256(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);
void fast10_avx2_blocking_512(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);
void fast10_avx2_blocking_1024(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);

void fast10_avx2_vecpeeling(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);
void fast10_avx2_vecpeeling_mask(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);

void fast10_avx2_checkposition(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);

void fast10_avx2_gather4x8(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);
void fast10_avx2_gather8x4(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);

void fast10_avx2_16x2(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);

void fast10_avx512_32x2(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);
void fast10_avx512_16x4(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);
void fast10_avx512_8x8(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier);