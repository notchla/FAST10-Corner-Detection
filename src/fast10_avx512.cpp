#include "fast.h"

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

void fast10_avx512(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier)
{
    #if AVX512_ENABLED
    const uint32_t stride = 3 * row_stride;

    uint8_t c_barrier = (uint8_t)barrier;
    __m512i barriers = _mm512_set1_epi8(c_barrier);

    int xend = width - 3;
    xend -= (width - 3) % 64;

    int yend = height -3;

    for(int y = 3; y < yend; y++)
    {

#if PEELING_ENABLED
        for (int x = 3; x < 64; x++) {
            const uint8_t* p = data + y * row_stride + x;
            if (CVD::is_corner_10<CVD::Less>(p, row_stride, barrier) || CVD::is_corner_10<CVD::Greater>(p, row_stride, barrier))
                corners.push_back(ImageRef(x, y));
        }
#endif

        for(int x = 64; x < xend; x += 64)
        {
#if COUNT_CHECKS
            check[0]++;
#endif

            const uint8_t* p = data + y * row_stride + x;

            __m512i lo, hi;
            {
                // lo = *p - c_barrier < 0 ? 0 : *p - c_barrier;
                // hi = *p + c_barrier > 255 ? 255 : *p + c_barrier;

                const __m512i v = _mm512_loadu_si512(p);
                lo = _mm512_subs_epu8(v, barriers);
                hi = _mm512_adds_epu8(barriers, v);
            }

#if COUNT_CHECKS
            check[1]++;
#endif
            uint128_t ans_b, ans_e;
            {
                // uint8_t top = *(p - stride);
                // uint8_t bottom = *(p + stride);

                __m512i top = _mm512_loadu_si512(p - stride);
                __m512i bottom = _mm512_loadu_si512(p + stride);

                CHECK_BARRIER_AXV512(lo, hi, top, ans_b);
                CHECK_BARRIER_AXV512(lo, hi, bottom, ans_e);

                if(!(ans_b | ans_e))
                    continue;
            }

#if COUNT_CHECKS
            check[2]++;
#endif
            uint128_t ans_m, ans_p, possible;
            {
                // uint8_t ul = *(p - 2 - 2 * row_stride);
                // uint8_t lr = *(p + 2 + 2 * row_stride);

                __m512i ul = _mm512_loadu_si512(p - 2 - 2 * row_stride);
                __m512i lr = _mm512_loadu_si512(p + 2 + 2 * row_stride);

                CHECK_BARRIER_AXV512(lo, hi, ul, ans_m);
                CHECK_BARRIER_AXV512(lo, hi, lr, ans_p);

                possible = (ans_m & ans_b) | (ans_e & ans_p);
                if(!possible)
                    continue;
            }


#if COUNT_CHECKS
            check[3]++;
#endif
            uint128_t ans_o, ans_n;
            {
                // uint8_t ll = *(p - 2 + 2 * row_stride);
                // uint8_t ur = *(p + 2 - 2 * row_stride);

                __m512i ll = _mm512_loadu_si512(p - 2 + 2 * row_stride);
                __m512i ur = _mm512_loadu_si512(p + 2 - 2 * row_stride);

                CHECK_BARRIER_AXV512(lo, hi, ll, ans_o);
                CHECK_BARRIER_AXV512(lo, hi, ur, ans_n);

                possible &= ans_o | (ans_b & ans_n);
                possible &= ans_n | (ans_e & ans_o);
                if(!possible)
                    continue;
            }

#if COUNT_CHECKS
            check[4]++;
#endif
            uint128_t ans_h, ans_k;
            {
                // uint8_t left = *(p - 3);
                // uint8_t right = *(p + 3);

                __m512i left = _mm512_loadu_si512(p - 3);
                __m512i right = _mm512_loadu_si512(p + 3);
                
                CHECK_BARRIER_AXV512(lo, hi, left, ans_h);
                CHECK_BARRIER_AXV512(lo, hi, right, ans_k);

                possible &= ans_h | (ans_n & ans_k & ans_p);
                possible &= ans_k | (ans_m & ans_h & ans_o);
                if(!possible)
                    continue;
            }

#if COUNT_CHECKS
            check[5]++;
#endif
            uint128_t ans_a, ans_c;
            {
                // uint8_t a = *(p - 1 - stride);
                // uint8_t c = *(p + 1 - stride);

                __m512i a = _mm512_loadu_si512(p - 1 - stride);
                __m512i c = _mm512_loadu_si512(p + 1 - stride);

                CHECK_BARRIER_AXV512(lo, hi, a, ans_a);
                CHECK_BARRIER_AXV512(lo, hi, c, ans_c);

                possible &= ans_a | (ans_e & ans_p);
                possible &= ans_c | (ans_o & ans_e);
                if(!possible)
                    continue;
            }

#if COUNT_CHECKS
            check[6]++;
#endif

            uint128_t ans_d, ans_f;
            {
                __m512i d = _mm512_loadu_si512(p - 1 + stride);
                __m512i f = _mm512_loadu_si512(p + 1 + stride);

                //__m256i f = _mm256_insert_epi16(_mm256_srli_si256(d, 2), *(const unsigned short*)(p + 15 + stride), 7);
                CHECK_BARRIER_AXV512(lo, hi, d, ans_d);
                CHECK_BARRIER_AXV512(lo, hi, f, ans_f);
                const uint128_t ans_abc = ans_a & ans_b & ans_c;
                possible &= ans_d | (ans_abc & ans_n);
                possible &= ans_f | (ans_m & ans_abc);
                if(!possible)
                    continue;
            }

#if COUNT_CHECKS
            check[7]++;
#endif
            uint128_t ans_g, ans_i;
            {
                // uint8_t g = *(p - 3 - row_stride);
                // uint8_t ii = *(p - 3 + row_stride);

                __m512i g = _mm512_loadu_si512(p - 3 - row_stride);
                __m512i ii = _mm512_loadu_si512(p - 3 + row_stride);

                CHECK_BARRIER_AXV512(lo, hi, g, ans_g);
                CHECK_BARRIER_AXV512(lo, hi, ii, ans_i);

                possible &= ans_g | (ans_f & ans_p & ans_k);
                possible &= ans_i | (ans_c & ans_n & ans_k);
                if(!possible)
                    continue;
            }

#if COUNT_CHECKS
            check[7]++;
#endif

            uint128_t ans_j, ans_l;
            {
                // uint8_t jj = *(p + 3 - row_stride);
                // uint8_t l = *(p + 3 + row_stride);

                __m512i jj = _mm512_loadu_si512(p + 3 - row_stride);
                __m512i l = _mm512_loadu_si512(p + 3 + row_stride);

                CHECK_BARRIER_AXV512(lo, hi, jj, ans_j);
                CHECK_BARRIER_AXV512(lo, hi, l, ans_l);

                const uint128_t ans_ghi = ans_g & ans_h & ans_i;
                possible &= ans_j | (ans_d & ans_o & ans_ghi);
                possible &= ans_l | (ans_m & ans_a & ans_ghi);
                if(!possible)
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

            for(size_t i = 0; i < 64; i+=8){
                __m512i data = _mm512_set_epi32(y, x + i + 7, y, x + i + 6, y, x + i + 5, y, x + i + 4, y, x + i + 3, y, x + i + 2, y, x + i + 1, y, x + i);
                __mmask8 mask_compress = possible >> i;
                int inserted = _popcnt32(mask_compress);
                _mm512_mask_compressstoreu_epi64(corner_ptr, mask_compress, data);
                corner_ptr += inserted;
            }



            // //todo change for 512
            // for (uint128_t i = 0; i < 64; i++) {
            //     if (possible & ((uint128_t)1 << i)) {
            //         corners.push_back(ImageRef(x + i, y));
            //     }
            // }

        }

#if PEELING_ENABLED
        for (int x = xend; x < width - 3; x++) {
            const uint8_t* p = data + y * row_stride + x;
            if (CVD::is_corner_10<CVD::Less>(p, row_stride, barrier) || CVD::is_corner_10<CVD::Greater>(p, row_stride, barrier))
                corners.push_back(ImageRef(x, y));
        }
#endif
    }
#else
    printf("avx512 not supported\n");
    fast10_avx2(data, width, height, row_stride, corners, barrier);
#endif
}
