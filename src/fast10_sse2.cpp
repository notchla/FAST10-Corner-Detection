#include "fast.h"



void fast10_sse2(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier)
{
    const int stride = 3 * row_stride;

    uint8_t c_barrier = (uint8_t)barrier;
    __m128i barriers = _mm_set1_epi8(c_barrier);


    int xend = width - 3;
    xend -= (width - 3) % 16;

    int yend = height - 3;

    for(int y = 3; y < yend; y++)
    {
#if PEELING_ENABLED
        for (int x = 3; x < 16; x++) {
            const uint8_t* p = data + y * row_stride + x;
            if (CVD::is_corner_10<CVD::Less>(p, row_stride, barrier) || CVD::is_corner_10<CVD::Greater>(p, row_stride, barrier))
                corners.push_back(ImageRef(x, y));
        }
#endif 

        for(int x = 16; x < xend; x += 16)
        {
#if COUNT_CHECKS
            check[0]++;
#endif

            const uint8_t* p = data + y * row_stride + x;
            //uint8_t lo, hi;

            __m128i lo, hi;

            {
                // lo = *p - c_barrier < 0 ? 0 : *p - c_barrier;
                // hi = *p + c_barrier > 255 ? 255 : *p + c_barrier;

                const __m128i v = _mm_load_si128((const __m128i*)(p));
                lo = _mm_subs_epu8(v, barriers);
                hi = _mm_adds_epu8(barriers, v);
            }

            unsigned int ans_b, ans_e;
            {
                // uint8_t top = *(p - stride);
                // uint8_t bottom = *(p + stride);

                __m128i top = _mm_load_si128((const __m128i*)(p - stride));
                __m128i bottom = _mm_load_si128((const __m128i*)(p + stride));

                CHECK_BARRIER(lo, hi, top, ans_b);
                CHECK_BARRIER(lo, hi, bottom, ans_e);

                if(!(ans_b | ans_e))
                    continue;
            }

#if COUNT_CHECKS
            check[1]++;
#endif
            unsigned int ans_m, ans_p, possible;
            {
                // uint8_t ul = *(p - 2 - 2 * row_stride);
                // uint8_t lr = *(p + 2 + 2 * row_stride);

                __m128i ul = _mm_loadu_si128((const __m128i*)(p - 2 - 2 * row_stride));
                __m128i lr = _mm_loadu_si128((const __m128i*)(p + 2 + 2 * row_stride));

                CHECK_BARRIER(lo, hi, ul, ans_m);
                CHECK_BARRIER(lo, hi, lr, ans_p);

                possible = (ans_m & ans_b) | (ans_e & ans_p);
                if(!possible)
                    continue;
            }


#if COUNT_CHECKS
            check[2]++;
#endif
            unsigned int ans_o, ans_n;
            {
                // uint8_t ll = *(p - 2 + 2 * row_stride);
                // uint8_t ur = *(p + 2 - 2 * row_stride);

                __m128i ll = _mm_loadu_si128((const __m128i*)(p - 2 + 2 * row_stride));
                __m128i ur = _mm_loadu_si128((const __m128i*)(p + 2 - 2 * row_stride));

                CHECK_BARRIER(lo, hi, ll, ans_o);
                CHECK_BARRIER(lo, hi, ur, ans_n);

                possible &= ans_o | (ans_b & ans_n);
                possible &= ans_n | (ans_e & ans_o);
                if(!possible)
                    continue;
            }

#if COUNT_CHECKS
            check[3]++;
#endif
            unsigned int ans_h, ans_k;
            {
                // uint8_t left = *(p - 3);
                // uint8_t right = *(p + 3);

                __m128i left = _mm_loadu_si128((const __m128i*)(p - 3));
                __m128i right = _mm_loadu_si128((const __m128i*)(p + 3));
                
                CHECK_BARRIER(lo, hi, left, ans_h);
                CHECK_BARRIER(lo, hi, right, ans_k);

                possible &= ans_h | (ans_n & ans_k & ans_p);
                possible &= ans_k | (ans_m & ans_h & ans_o);
                if(!possible)
                    continue;
            }

#if COUNT_CHECKS
            check[4]++;
#endif
            unsigned int ans_a, ans_c;
            {
                // uint8_t a = *(p - 1 - stride);
                // uint8_t c = *(p + 1 - stride);

                __m128i a = _mm_loadu_si128((const __m128i*)(p - 1 - stride));
                __m128i c = _mm_loadu_si128((const __m128i*)(p + 1 - stride));

                CHECK_BARRIER(lo, hi, a, ans_a);
                CHECK_BARRIER(lo, hi, c, ans_c);

                possible &= ans_a | (ans_e & ans_p);
                possible &= ans_c | (ans_o & ans_e);
                if(!possible)
                    continue;
            }

#if COUNT_CHECKS
            check[5]++;
#endif

            unsigned int ans_d, ans_f;
            {
                __m128i d = _mm_loadu_si128((const __m128i*)(p - 1 + stride));
                __m128i f = _mm_insert_epi16(_mm_srli_si128(d, 2), *(const unsigned short*)(p + 15 + stride), 7);
                //__m128i f = _mm_loadu_si128((const __m128i*)(p+1+stride));
                CHECK_BARRIER(lo, hi, d, ans_d);
                CHECK_BARRIER(lo, hi, f, ans_f);
                const unsigned int ans_abc = ans_a & ans_b & ans_c;
                possible &= ans_d | (ans_abc & ans_n);
                possible &= ans_f | (ans_m & ans_abc);
                if(!possible)
                    continue;
            }

#if COUNT_CHECKS
            check[6]++;
#endif
            unsigned int ans_g, ans_i;
            {
                // uint8_t g = *(p - 3 - row_stride);
                // uint8_t ii = *(p - 3 + row_stride);

                __m128i g = _mm_loadu_si128((const __m128i*)(p - 3 - row_stride));
                __m128i ii = _mm_loadu_si128((const __m128i*)(p - 3 + row_stride));

                CHECK_BARRIER(lo, hi, g, ans_g);
                CHECK_BARRIER(lo, hi, ii, ans_i);

                possible &= ans_g | (ans_f & ans_p & ans_k);
                possible &= ans_i | (ans_c & ans_n & ans_k);
                if(!possible)
                    continue;
            }

#if COUNT_CHECKS
            check[7]++;
#endif

            unsigned int ans_j, ans_l;
            {
                // uint8_t jj = *(p + 3 - row_stride);
                // uint8_t l = *(p + 3 + row_stride);

                __m128i jj = _mm_loadu_si128((const __m128i*)(p + 3 - row_stride));
                __m128i l = _mm_loadu_si128((const __m128i*)(p + 3 + row_stride));

                CHECK_BARRIER(lo, hi, jj, ans_j);
                CHECK_BARRIER(lo, hi, l, ans_l);

                const unsigned int ans_ghi = ans_g & ans_h & ans_i;
                possible &= ans_j | (ans_d & ans_o & ans_ghi);
                possible &= ans_l | (ans_m & ans_a & ans_ghi);
                if(!possible)
                    continue;
            }

#if COUNT_CHECKS
            check[8]++;
#endif

            possible |= (possible >> 16);
#if 1
            for (uint64_t i = 0; i < 16; i++) {
                if (possible & (1ull << i)) {
#if COUNT_CHECKS
            check[9]++;
#endif
                    corners.push_back(ImageRef(x + i, y));
                }
            }
#else
            //if(possible & 0x0f) //Does this make it faster?
            {
                if(possible & (1 << 0))
                    corners.push_back(ImageRef(x + 0, y));
                if(possible & (1 << 1))
                    corners.push_back(ImageRef(x + 1, y));
                if(possible & (1 << 2))
                    corners.push_back(ImageRef(x + 2, y));
                if(possible & (1 << 3))
                    corners.push_back(ImageRef(x + 3, y));
                if(possible & (1 << 4))
                    corners.push_back(ImageRef(x + 4, y));
                if(possible & (1 << 5))
                    corners.push_back(ImageRef(x + 5, y));
                if(possible & (1 << 6))
                    corners.push_back(ImageRef(x + 6, y));
                if(possible & (1 << 7))
                    corners.push_back(ImageRef(x + 7, y));
            }
            //if(possible & 0xf0) //Does this mak( ,  fast)r?
            {
                if(possible & (1 << 8))
                    corners.push_back(ImageRef(x + 8, y));
                if(possible & (1 << 9))
                    corners.push_back(ImageRef(x + 9, y));
                if(possible & (1 << 10))
                    corners.push_back(ImageRef(x + 10, y));
                if(possible & (1 << 11))
                    corners.push_back(ImageRef(x + 11, y));
                if(possible & (1 << 12))
                    corners.push_back(ImageRef(x + 12, y));
                if(possible & (1 << 13))
                    corners.push_back(ImageRef(x + 13, y));
                if(possible & (1 << 14))
                    corners.push_back(ImageRef(x + 14, y));
                if(possible & (1 << 15))
                    corners.push_back(ImageRef(x + 15, y));
            }
#endif

        }

#if PEELING_ENABLED
        for(int x = xend; x < width - 3; x++) {
            const uint8_t* p = data + y * row_stride + x;
            if (CVD::is_corner_10<CVD::Less>(p, row_stride, barrier) || CVD::is_corner_10<CVD::Greater>(p, row_stride, barrier))
                corners.push_back(ImageRef(x, y));
        }
#endif
    }
}
