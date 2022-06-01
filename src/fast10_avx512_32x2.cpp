#include "fast.h"

#if AVX512_ENABLED

typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;

#define CHECK_BARRIER_AXV512(lo, hi, other, flags)                                  \
    {                                                                             \
        __m512i diff = _mm512_subs_epu8(lo, other);                               \
        __m512i diff2 = _mm512_subs_epu8(other, hi);                              \
        __m512i z = _mm512_setzero_si512();                                       \
        __mmask64 diff_mask = _mm512_cmpeq_epi8_mask(diff, z);                                        \
        __mmask64 diff2_mask = _mm512_cmpeq_epi8_mask(diff2, z);                                      \
        uint128_t m1 = diff_mask  & 0xFFFFFFFFFFFFFFFF; \
        uint128_t m2 = diff2_mask & 0xFFFFFFFFFFFFFFFF; \
        flags = ~(m1 | (m2 << 64)); \
    }

//#define LOAD(p) (_mm512_loadu_si512(p))
#define LOAD(p) (  \
    _mm512_inserti64x4( \
        _mm512_castsi256_si512(_mm256_loadu_si256((__m256i*)(p))),  \
        _mm256_loadu_si256((__m256i*)((p) + row_stride)), 1)  \
    ) 

void fast10_avx512_32x2(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier)
{
    const uint32_t stride = 3 * row_stride;

    uint8_t c_barrier = (uint8_t)barrier;
    __m512i barriers = _mm512_set1_epi8(c_barrier);

    int xend = width - 3;
    int yend = height - 3;

    xend -= xend % 64;
    yend -= (yend - 3) % 2;

#if PEELING_ENABLED
    for (int y = 3; y < yend; y++)
    {
        for (int x = 3; x < 64; x++) {
            const uint8_t* p = data + y * row_stride + x;
            if (CVD::is_corner_10<CVD::Less>(p, row_stride, barrier) || CVD::is_corner_10<CVD::Greater>(p, row_stride, barrier))
                corners.push_back(ImageRef(x, y));
        }
    }
#endif

    for (int y = 3; y < yend; y += 2) {
        for (int x = 64; x < xend; x += 32)
        {
#if COUNT_CHECKS
            check[0]++;
#endif

            const uint8_t* p = data + y * row_stride + x;

            __m512i lo, hi;
            {
                // lo = *p - c_barrier < 0 ? 0 : *p - c_barrier;
                // hi = *p + c_barrier > 255 ? 255 : *p + c_barrier;

                const __m512i v = LOAD(p);
                lo = _mm512_subs_epu8(v, barriers);
                hi = _mm512_adds_epu8(barriers, v);
            }

            uint128_t ans_b, ans_e;
            {
                // uint8_t top = *(p - stride);
                // uint8_t bottom = *(p + stride);

                __m512i top = LOAD(p - stride);
                __m512i bottom = LOAD(p + stride);

                CHECK_BARRIER_AXV512(lo, hi, top, ans_b);
                CHECK_BARRIER_AXV512(lo, hi, bottom, ans_e);

                if (!(ans_b | ans_e))
                    continue;
            }

#if COUNT_CHECKS
            check[1]++;
#endif
            uint128_t ans_m, ans_p, possible;
            {
                // uint8_t ul = *(p - 2 - 2 * row_stride);
                // uint8_t lr = *(p + 2 + 2 * row_stride);

                __m512i ul = LOAD(p - 2 - 2 * row_stride);
                __m512i lr = LOAD(p + 2 + 2 * row_stride);

                CHECK_BARRIER_AXV512(lo, hi, ul, ans_m);
                CHECK_BARRIER_AXV512(lo, hi, lr, ans_p);

                possible = (ans_m & ans_b) | (ans_e & ans_p);
                if (!possible)
                    continue;
            }


#if COUNT_CHECKS
            check[2]++;
#endif
            uint128_t ans_o, ans_n;
            {
                // uint8_t ll = *(p - 2 + 2 * row_stride);
                // uint8_t ur = *(p + 2 - 2 * row_stride);

                __m512i ll = LOAD(p - 2 + 2 * row_stride);
                __m512i ur = LOAD(p + 2 - 2 * row_stride);

                CHECK_BARRIER_AXV512(lo, hi, ll, ans_o);
                CHECK_BARRIER_AXV512(lo, hi, ur, ans_n);

                possible &= ans_o | (ans_b & ans_n);
                possible &= ans_n | (ans_e & ans_o);
                if (!possible)
                    continue;
            }

#if COUNT_CHECKS
            check[3]++;
#endif
            uint128_t ans_h, ans_k;
            {
                // uint8_t left = *(p - 3);
                // uint8_t right = *(p + 3);

                __m512i left = LOAD(p - 3);
                __m512i right = LOAD(p + 3);

                CHECK_BARRIER_AXV512(lo, hi, left, ans_h);
                CHECK_BARRIER_AXV512(lo, hi, right, ans_k);

                possible &= ans_h | (ans_n & ans_k & ans_p);
                possible &= ans_k | (ans_m & ans_h & ans_o);
                if (!possible)
                    continue;
            }

#if COUNT_CHECKS
            check[4]++;
#endif
            uint128_t ans_a, ans_c;
            {
                // uint8_t a = *(p - 1 - stride);
                // uint8_t c = *(p + 1 - stride);

                __m512i a = LOAD(p - 1 - stride);
                __m512i c = LOAD(p + 1 - stride);

                CHECK_BARRIER_AXV512(lo, hi, a, ans_a);
                CHECK_BARRIER_AXV512(lo, hi, c, ans_c);

                possible &= ans_a | (ans_e & ans_p);
                possible &= ans_c | (ans_o & ans_e);
                if (!possible)
                    continue;
            }

#if COUNT_CHECKS
            check[5]++;
#endif

            uint128_t ans_d, ans_f;
            {
                __m512i d = LOAD(p - 1 + stride);
                __m512i f = LOAD(p + 1 + stride);

                //__m256i f = _mm256_insert_epi16(_mm256_srli_si256(d, 2), *(const unsigned short*)(p + 15 + stride), 7);
                CHECK_BARRIER_AXV512(lo, hi, d, ans_d);
                CHECK_BARRIER_AXV512(lo, hi, f, ans_f);
                const uint128_t ans_abc = ans_a & ans_b & ans_c;
                possible &= ans_d | (ans_abc & ans_n);
                possible &= ans_f | (ans_m & ans_abc);
                if (!possible)
                    continue;
            }

#if COUNT_CHECKS
            check[6]++;
#endif
            uint128_t ans_g, ans_i;
            {
                // uint8_t g = *(p - 3 - row_stride);
                // uint8_t ii = *(p - 3 + row_stride);

                __m512i g = LOAD(p - 3 - row_stride);
                __m512i ii = LOAD(p - 3 + row_stride);

                CHECK_BARRIER_AXV512(lo, hi, g, ans_g);
                CHECK_BARRIER_AXV512(lo, hi, ii, ans_i);

                possible &= ans_g | (ans_f & ans_p & ans_k);
                possible &= ans_i | (ans_c & ans_n & ans_k);
                if (!possible)
                    continue;
            }

#if COUNT_CHECKS
            check[7]++;
#endif

            uint128_t ans_j, ans_l;
            {
                // uint8_t jj = *(p + 3 - row_stride);
                // uint8_t l = *(p + 3 + row_stride);

                __m512i jj = LOAD(p + 3 - row_stride);
                __m512i l = LOAD(p + 3 + row_stride);

                CHECK_BARRIER_AXV512(lo, hi, jj, ans_j);
                CHECK_BARRIER_AXV512(lo, hi, l, ans_l);

                const uint128_t ans_ghi = ans_g & ans_h & ans_i;
                possible &= ans_j | (ans_d & ans_o & ans_ghi);
                possible &= ans_l | (ans_m & ans_a & ans_ghi);
                if (!possible)
                    continue;
            }

#if COUNT_CHECKS
            check[8]++;
#endif

            possible |= (possible >> 64);

            uint64_t possible_64 = (uint64_t)possible;

            int n_corners = _popcnt64(possible_64);

            size_t size = corners.size();

            corners.resize(corners.size() + n_corners);

            ImageRef* corner_ptr = corners.data() + size;

            for (size_t j = 0; j < 2; j++) {
                for (size_t i = 0; i < 32; i += 8) {
                    __m512i data = _mm512_set_epi32(y + j, x + i + 7, y + j, x + i + 6, y + j, x + i + 5, y + j, x + i + 4, y + j, x + i + 3, y + j, x + i + 2, y + j, x + i + 1, y + j, x + i);
                    __mmask8 mask_compress = possible; 
                    int inserted = _popcnt32(mask_compress);
                    _mm512_mask_compressstoreu_epi64(corner_ptr, mask_compress, data);

                    corner_ptr += inserted;
                    possible = possible >> 8;
                }
            }




            // //todo change for 512
            // for (uint128_t i = 0; i < 64; i++) {
            //     if (possible & ((uint128_t)1 << i)) {
            //         corners.push_back(ImageRef(x + i, y));
            //     }
            // }

        }
    }

#if PEELING_ENABLED
    for (int y = 3; y < yend; y++)
    {
        for (int x = xend; x < width - 3; x++) {
            const uint8_t* p = data + y * row_stride + x;
            if (CVD::is_corner_10<CVD::Less>(p, row_stride, barrier) || CVD::is_corner_10<CVD::Greater>(p, row_stride, barrier))
                corners.push_back(ImageRef(x, y));
        }
    }

    for (int y = yend; y < height - 3; y++)
    {
        for (int x = 3; x < width - 3; x++) {
            const uint8_t* p = data + y * row_stride + x;
            if (CVD::is_corner_10<CVD::Less>(p, row_stride, barrier) || CVD::is_corner_10<CVD::Greater>(p, row_stride, barrier))
                corners.push_back(ImageRef(x, y));
        }
    }
#endif
}

#endif
