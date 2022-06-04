#include "fast.h"

#define CHECK_BARRIER_AXV2(lo, hi, other, flags)                                  \
    {                                                                             \
        __m256i diff = _mm256_subs_epu8(lo, other);                               \
        __m256i diff2 = _mm256_subs_epu8(other, hi);                              \
        __m256i z = _mm256_setzero_si256();                                       \
        diff = _mm256_cmpeq_epi8(diff, z);                                        \
        diff2 = _mm256_cmpeq_epi8(diff2, z);                                      \
        uint64_t m1 = _mm256_movemask_epi8(diff)  & 0xFFFFFFFF; \
        uint64_t m2 = _mm256_movemask_epi8(diff2) & 0xFFFFFFFF; \
        flags = ~(m1 | (m2 << 32)); \
    }

#define FUNCNAME_(b) fast10_avx2_blocking_##b
#define FUNCNAME(b) FUNCNAME_(b)

#define BLOCK_SIZE 1024
#include "fast10_avx2_blocking.inl"

#undef BLOCK_SIZE
#define BLOCK_SIZE 512
#include "fast10_avx2_blocking.inl"

#undef BLOCK_SIZE
#define BLOCK_SIZE 256
#include "fast10_avx2_blocking.inl"
