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

void fast10_avx2_unrolled_3(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier)
{
    const uint32_t stride = 3 * row_stride;

    uint8_t c_barrier = (uint8_t)barrier;
    __m256i barriers = _mm256_set1_epi8(c_barrier);

    int xend = width - 3;
    xend -= (width - 3 - 32) % 96;

    int yend = height -3;

    for(int y = 3; y < yend; y++)
    {

#if PEELING_ENABLED
        for (int x = 3; x < 32; x++) {
            const uint8_t* p = data + y * row_stride + x;
            if (CVD::is_corner_10<CVD::Less>(p, row_stride, barrier) || CVD::is_corner_10<CVD::Greater>(p, row_stride, barrier))
                corners.push_back(ImageRef(x, y));
        }
#endif

        for(int x = 32; x < xend; x += 96)
        {
#if COUNT_CHECKS
            check[0] += 2;
#endif

            const uint8_t* p = data + y * row_stride + x;
            const uint8_t* p_2 = data + y * row_stride + x + 32;
            const uint8_t* p_3 = data + y * row_stride + x + 64;

            __m256i lo, hi, lo_2, hi_2, lo_3, hi_3;
            {
                // lo = *p - c_barrier < 0 ? 0 : *p - c_barrier;
                // hi = *p + c_barrier > 255 ? 255 : *p + c_barrier;

                const __m256i v = _mm256_loadu_si256((const __m256i*)(p));
                lo = _mm256_subs_epu8(v, barriers);
                hi = _mm256_adds_epu8(barriers, v);
                const __m256i v_2 = _mm256_loadu_si256((const __m256i*)(p_2));
                lo_2 = _mm256_subs_epu8(v_2, barriers);
                hi_2 = _mm256_adds_epu8(barriers, v_2);
                const __m256i v_3 = _mm256_loadu_si256((const __m256i*)(p_3));
                lo_3 = _mm256_subs_epu8(v_3, barriers);
                hi_3 = _mm256_adds_epu8(barriers, v_3);
            }

            uint64_t ans_b, ans_e, ans_b_2, ans_e_2, ans_b_3, ans_e_3;
            {
                // uint8_t top = *(p - stride);
                // uint8_t bottom = *(p + stride);

                __m256i top = _mm256_loadu_si256((const __m256i*)(p - stride));
                __m256i bottom = _mm256_loadu_si256((const __m256i*)(p + stride));
                __m256i top_2 = _mm256_loadu_si256((const __m256i*)(p_2 - stride));
                __m256i bottom_2 = _mm256_loadu_si256((const __m256i*)(p_2 + stride));
                __m256i top_3 = _mm256_loadu_si256((const __m256i*)(p_3 - stride));
                __m256i bottom_3 = _mm256_loadu_si256((const __m256i*)(p_3 + stride));

                CHECK_BARRIER_AXV2(lo, hi, top, ans_b);
                CHECK_BARRIER_AXV2(lo, hi, bottom, ans_e);
                CHECK_BARRIER_AXV2(lo_2, hi_2, top_2, ans_b_2);
                CHECK_BARRIER_AXV2(lo_2, hi_2, bottom_2, ans_e_2);
                CHECK_BARRIER_AXV2(lo_3, hi_3, top_3, ans_b_3);
                CHECK_BARRIER_AXV2(lo_3, hi_3, bottom_3, ans_e_3);

                if(!(ans_b | ans_e) && !(ans_b_2 | ans_e_2) && !(ans_b_3 | ans_e_3))
                    continue;
            }

#if COUNT_CHECKS
            check[1] += 3;
#endif
            uint64_t ans_m, ans_p, possible, ans_m_2, ans_p_2, possible_2, ans_m_3, ans_p_3, possible_3;
            {
                // uint8_t ul = *(p - 2 - 2 * row_stride);
                // uint8_t lr = *(p + 2 + 2 * row_stride);

                __m256i ul = _mm256_loadu_si256((const __m256i*)(p - 2 - 2 * row_stride));
                __m256i lr = _mm256_loadu_si256((const __m256i*)(p + 2 + 2 * row_stride));
                __m256i ul_2 = _mm256_loadu_si256((const __m256i*)(p_2 - 2 - 2 * row_stride));
                __m256i lr_2 = _mm256_loadu_si256((const __m256i*)(p_2 + 2 + 2 * row_stride));
                __m256i ul_3 = _mm256_loadu_si256((const __m256i*)(p_3 - 2 - 2 * row_stride));
                __m256i lr_3 = _mm256_loadu_si256((const __m256i*)(p_3 + 2 + 2 * row_stride));

                CHECK_BARRIER_AXV2(lo, hi, ul, ans_m);
                CHECK_BARRIER_AXV2(lo, hi, lr, ans_p);
                CHECK_BARRIER_AXV2(lo_2, hi_2, ul_2, ans_m_2);
                CHECK_BARRIER_AXV2(lo_2, hi_2, lr_2, ans_p_2);
                CHECK_BARRIER_AXV2(lo_3, hi_3, ul_3, ans_m_3);
                CHECK_BARRIER_AXV2(lo_3, hi_3, lr_3, ans_p_3);

                possible = (ans_m & ans_b) | (ans_e & ans_p);
                possible_2 = (ans_m_2 & ans_b_2) | (ans_e_2 & ans_p_2);
                possible_3 = (ans_m_3 & ans_b_3) | (ans_e_3 & ans_p_3);
                if (!possible && !possible_2 && !possible_3)
                    continue;
            }


#if COUNT_CHECKS
            check[2] += 3;
#endif
            uint64_t ans_o, ans_n, ans_o_2, ans_n_2, ans_o_3, ans_n_3;
            {
                // uint8_t ll = *(p - 2 + 2 * row_stride);
                // uint8_t ur = *(p + 2 - 2 * row_stride);

                __m256i ll = _mm256_loadu_si256((const __m256i*)(p - 2 + 2 * row_stride));
                __m256i ur = _mm256_loadu_si256((const __m256i*)(p + 2 - 2 * row_stride));
                __m256i ll_2 = _mm256_loadu_si256((const __m256i*)(p_2 - 2 + 2 * row_stride));
                __m256i ur_2 = _mm256_loadu_si256((const __m256i*)(p_2 + 2 - 2 * row_stride));
                __m256i ll_3 = _mm256_loadu_si256((const __m256i*)(p_3 - 2 + 2 * row_stride));
                __m256i ur_3 = _mm256_loadu_si256((const __m256i*)(p_3 + 2 - 2 * row_stride));

                CHECK_BARRIER_AXV2(lo, hi, ll, ans_o);
                CHECK_BARRIER_AXV2(lo, hi, ur, ans_n);
                CHECK_BARRIER_AXV2(lo_2, hi_2, ll_2, ans_o_2);
                CHECK_BARRIER_AXV2(lo_2, hi_2, ur_2, ans_n_2);
                CHECK_BARRIER_AXV2(lo_3, hi_3, ll_3, ans_o_3);
                CHECK_BARRIER_AXV2(lo_3, hi_3, ur_3, ans_n_3);

                possible &= ans_o | (ans_b & ans_n);
                possible &= ans_n | (ans_e & ans_o);
                possible_2 &= ans_o_2 | (ans_b_2 & ans_n_2);
                possible_2 &= ans_n_2 | (ans_e_2 & ans_o_2);
                possible_3 &= ans_o_3 | (ans_b_3 & ans_n_3);
                possible_3 &= ans_n_3 | (ans_e_3 & ans_o_3);
                if (!possible && !possible_2 && !possible_3)
                    continue;
            }

#if COUNT_CHECKS
            check[3] += 3;
#endif
            uint64_t ans_h, ans_k, ans_h_2, ans_k_2, ans_h_3, ans_k_3;
            {
                // uint8_t left = *(p - 3);
                // uint8_t right = *(p + 3);

                __m256i left = _mm256_loadu_si256((const __m256i*)(p - 3));
                __m256i right = _mm256_loadu_si256((const __m256i*)(p + 3));
                __m256i left_2 = _mm256_loadu_si256((const __m256i*)(p_2 - 3));
                __m256i right_2 = _mm256_loadu_si256((const __m256i*)(p_2 + 3));
                __m256i left_3 = _mm256_loadu_si256((const __m256i*)(p_3 - 3));
                __m256i right_3 = _mm256_loadu_si256((const __m256i*)(p_3 + 3));
                
                CHECK_BARRIER_AXV2(lo, hi, left, ans_h);
                CHECK_BARRIER_AXV2(lo, hi, right, ans_k);
                CHECK_BARRIER_AXV2(lo_2, hi_2, left_2, ans_h_2);
                CHECK_BARRIER_AXV2(lo_2, hi_2, right_2, ans_k_2);
                CHECK_BARRIER_AXV2(lo_3, hi_3, left_3, ans_h_3);
                CHECK_BARRIER_AXV2(lo_3, hi_3, right_3, ans_k_3);

                possible &= ans_h | (ans_n & ans_k & ans_p);
                possible &= ans_k | (ans_m & ans_h & ans_o);
                possible_2 &= ans_h_2 | (ans_n_2 & ans_k_2 & ans_p_2);
                possible_2 &= ans_k_2 | (ans_m_2 & ans_h_2 & ans_o_2);
                possible_3 &= ans_h_3 | (ans_n_3 & ans_k_3 & ans_p_3);
                possible_3 &= ans_k_3 | (ans_m_3 & ans_h_3 & ans_o_3);
                if (!possible && !possible_2 && !possible_3)
                    continue;
            }

#if COUNT_CHECKS
            check[4] += 3;
#endif
            uint64_t ans_a, ans_c, ans_a_2, ans_c_2, ans_a_3, ans_c_3;
            {
                // uint8_t a = *(p - 1 - stride);
                // uint8_t c = *(p + 1 - stride);

                __m256i a = _mm256_loadu_si256((const __m256i*)(p - 1 - stride));
                __m256i c = _mm256_loadu_si256((const __m256i*)(p + 1 - stride));
                __m256i a_2 = _mm256_loadu_si256((const __m256i*)(p_2 - 1 - stride));
                __m256i c_2 = _mm256_loadu_si256((const __m256i*)(p_2 + 1 - stride));
                __m256i a_3 = _mm256_loadu_si256((const __m256i*)(p_3 - 1 - stride));
                __m256i c_3 = _mm256_loadu_si256((const __m256i*)(p_3 + 1 - stride));

                CHECK_BARRIER_AXV2(lo, hi, a, ans_a);
                CHECK_BARRIER_AXV2(lo, hi, c, ans_c);
                CHECK_BARRIER_AXV2(lo_2, hi_2, a_2, ans_a_2);
                CHECK_BARRIER_AXV2(lo_2, hi_2, c_2, ans_c_2);
                CHECK_BARRIER_AXV2(lo_3, hi_3, a_3, ans_a_3);
                CHECK_BARRIER_AXV2(lo_3, hi_3, c_3, ans_c_3);

                possible &= ans_a | (ans_e & ans_p);
                possible &= ans_c | (ans_o & ans_e);
                possible_2 &= ans_a_2 | (ans_e_2 & ans_p_2);
                possible_2 &= ans_c_2 | (ans_o_2 & ans_e_2);
                possible_3 &= ans_a_3 | (ans_e_3 & ans_p_3);
                possible_3 &= ans_c_3 | (ans_o_3 & ans_e_3);
                if (!possible && !possible_2 && !possible_3)
                    continue;
            }

#if COUNT_CHECKS
            check[5] += 3;
#endif

            uint64_t ans_d, ans_f, ans_d_2, ans_f_2, ans_d_3, ans_f_3;
            {
                __m256i d = _mm256_loadu_si256((const __m256i*)(p - 1 + stride));
                __m256i f = _mm256_loadu_si256((const __m256i*)(p + 1 + stride));
                __m256i d_2 = _mm256_loadu_si256((const __m256i*)(p_2 - 1 + stride));
                __m256i f_2 = _mm256_loadu_si256((const __m256i*)(p_2 + 1 + stride));
                __m256i d_3 = _mm256_loadu_si256((const __m256i*)(p_3 - 1 + stride));
                __m256i f_3 = _mm256_loadu_si256((const __m256i*)(p_3 + 1 + stride));

                //__m256i f = _mm256_insert_epi16(_mm256_srli_si256(d, 2), *(const unsigned short*)(p + 15 + stride), 7);
                CHECK_BARRIER_AXV2(lo, hi, d, ans_d);
                CHECK_BARRIER_AXV2(lo, hi, f, ans_f);
                CHECK_BARRIER_AXV2(lo_2, hi_2, d_2, ans_d_2);
                CHECK_BARRIER_AXV2(lo_2, hi_2, f_2, ans_f_2);
                CHECK_BARRIER_AXV2(lo_3, hi_3, d_3, ans_d_3);
                CHECK_BARRIER_AXV2(lo_3, hi_3, f_3, ans_f_3);
                const uint64_t ans_abc = ans_a & ans_b & ans_c;
                possible &= ans_d | (ans_abc & ans_n);
                possible &= ans_f | (ans_m & ans_abc);
                const uint64_t ans_abc_2 = ans_a_2 & ans_b_2 & ans_c_2;
                possible_2 &= ans_d_2 | (ans_abc_2 & ans_n_2);
                possible_2 &= ans_f_2 | (ans_m_2 & ans_abc_2);
                const uint64_t ans_abc_3 = ans_a_3 & ans_b_3 & ans_c_3;
                possible_3 &= ans_d_3 | (ans_abc_3 & ans_n_3);
                possible_3 &= ans_f_3 | (ans_m_3 & ans_abc_3);
                if (!possible && !possible_2 && !possible_3)
                    continue;
            }

#if COUNT_CHECKS
            check[6] += 3;
#endif
            uint64_t ans_g, ans_i, ans_g_2, ans_i_2, ans_g_3, ans_i_3;
            {
                // uint8_t g = *(p - 3 - row_stride);
                // uint8_t ii = *(p - 3 + row_stride);

                __m256i g = _mm256_loadu_si256((const __m256i*)(p - 3 - row_stride));
                __m256i ii = _mm256_loadu_si256((const __m256i*)(p - 3 + row_stride));
                __m256i g_2 = _mm256_loadu_si256((const __m256i*)(p_2 - 3 - row_stride));
                __m256i ii_2 = _mm256_loadu_si256((const __m256i*)(p_2 - 3 + row_stride));
                __m256i g_3 = _mm256_loadu_si256((const __m256i*)(p_3 - 3 - row_stride));
                __m256i ii_3 = _mm256_loadu_si256((const __m256i*)(p_3 - 3 + row_stride));

                CHECK_BARRIER_AXV2(lo, hi, g, ans_g);
                CHECK_BARRIER_AXV2(lo, hi, ii, ans_i);
                CHECK_BARRIER_AXV2(lo_2, hi_2, g_2, ans_g_2);
                CHECK_BARRIER_AXV2(lo_2, hi_2, ii_2, ans_i_2);
                CHECK_BARRIER_AXV2(lo_3, hi_3, g_3, ans_g_3);
                CHECK_BARRIER_AXV2(lo_3, hi_3, ii_3, ans_i_3);

                possible &= ans_g | (ans_f & ans_p & ans_k);
                possible &= ans_i | (ans_c & ans_n & ans_k);
                possible_2 &= ans_g_2 | (ans_f_2 & ans_p_2 & ans_k_2);
                possible_2 &= ans_i_2 | (ans_c_2 & ans_n_2 & ans_k_2);
                possible_3 &= ans_g_3 | (ans_f_3 & ans_p_3 & ans_k_3);
                possible_3 &= ans_i_3 | (ans_c_3 & ans_n_3 & ans_k_3);
                if (!possible && !possible_2 && !possible_3)
                    continue;
            }

#if COUNT_CHECKS
            check[7] += 3;
#endif

            uint64_t ans_j, ans_l, ans_j_2, ans_l_2, ans_j_3, ans_l_3;
            {
                // uint8_t jj = *(p + 3 - row_stride);
                // uint8_t l = *(p + 3 + row_stride);

                __m256i jj = _mm256_loadu_si256((const __m256i*)(p + 3 - row_stride));
                __m256i l = _mm256_loadu_si256((const __m256i*)(p + 3 + row_stride));
                __m256i jj_2 = _mm256_loadu_si256((const __m256i*)(p_2 + 3 - row_stride));
                __m256i l_2 = _mm256_loadu_si256((const __m256i*)(p_2 + 3 + row_stride));
                __m256i jj_3 = _mm256_loadu_si256((const __m256i*)(p_3 + 3 - row_stride));
                __m256i l_3 = _mm256_loadu_si256((const __m256i*)(p_3 + 3 + row_stride));

                CHECK_BARRIER_AXV2(lo, hi, jj, ans_j);
                CHECK_BARRIER_AXV2(lo, hi, l, ans_l);
                CHECK_BARRIER_AXV2(lo_2, hi_2, jj_2, ans_j_2);
                CHECK_BARRIER_AXV2(lo_2, hi_2, l_2, ans_l_2);
                CHECK_BARRIER_AXV2(lo_3, hi_3, jj_3, ans_j_3);
                CHECK_BARRIER_AXV2(lo_3, hi_3, l_3, ans_l_3);

                const uint64_t ans_ghi = ans_g & ans_h & ans_i;
                possible &= ans_j | (ans_d & ans_o & ans_ghi);
                possible &= ans_l | (ans_m & ans_a & ans_ghi);
                const uint64_t ans_ghi_2 = ans_g_2 & ans_h_2 & ans_i_2;
                possible_2 &= ans_j_2 | (ans_d_2 & ans_o_2 & ans_ghi_2);
                possible_2 &= ans_l_2 | (ans_m_2 & ans_a_2 & ans_ghi_2);
                const uint64_t ans_ghi_3 = ans_g_3 & ans_h_3 & ans_i_3;
                possible_3 &= ans_j_3 | (ans_d_3 & ans_o_3 & ans_ghi_3);
                possible_3 &= ans_l_3 | (ans_m_3 & ans_a_3 & ans_ghi_3);
                if (!possible && !possible_2 && !possible_3)
                    continue;
            }

#if COUNT_CHECKS
            check[8] += 3;
#endif

            possible |= (possible >> 32);
            possible_2 |= (possible_2 >> 32);
            possible_3 |= (possible_3 >> 32);

            for (uint64_t i = 0; i < 32; i++) {
                if (possible & ((uint64_t)1 << i)) {
#if COUNT_CHECKS
                check[9]++;
#endif
                    corners.push_back(ImageRef(x + i, y));
                }
            }

            for (uint64_t i = 0; i < 32; i++) {
                if (possible_2 & ((uint64_t)1 << i)) {
#if COUNT_CHECKS
                check[9]++;
#endif
                    corners.push_back(ImageRef(x + 32 + i, y));
                }
            }

            for (uint64_t i = 0; i < 32; i++) {
                if (possible_3 & ((uint64_t)1 << i)) {
#if COUNT_CHECKS
                    check[9]++;
#endif
                    corners.push_back(ImageRef(x + 64 + i, y));
                }
            }

        }

#if PEELING_ENABLED
        for (int x = xend; x < width - 3; x++) {
            const uint8_t* p = data + y * row_stride + x;
            if (CVD::is_corner_10<CVD::Less>(p, row_stride, barrier) || CVD::is_corner_10<CVD::Greater>(p, row_stride, barrier))
                corners.push_back(ImageRef(x, y));
        }
#endif
    }
}
