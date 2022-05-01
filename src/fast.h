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
#else
#include <x86intrin.h>
#endif

#define COUNT_CHECKS true
extern uint64_t check[9];

using CVD::ImageRef;

inline uint64_t rdtsc() {
    return __rdtsc();
}

namespace CVD {
    void fast_corner_detect_plain_9(const BasicImage<byte>& i, std::vector<ImageRef>& corners, int b);
#include <cvd_src/corner_9.h>
}

typedef void fast_func(const CVD::BasicImage<CVD::byte>& I, std::vector<ImageRef>& corners, const int barrier);

void fast9_sse2(const CVD::BasicImage<CVD::byte>& I, std::vector<ImageRef>& corners, const int barrier);
void fast9_scalar(const CVD::BasicImage<CVD::byte>& I, std::vector<ImageRef>& corners, const int barrier);