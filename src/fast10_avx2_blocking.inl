void FUNCNAME(BLOCK_SIZE)(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier)
{
    const uint32_t stride = 3 * row_stride;

    uint8_t c_barrier = (uint8_t)barrier;
    __m256i barriers = _mm256_set1_epi8(c_barrier);

    int xend = width - 3;
    xend -= (width - 3) % 32;

    int yend = height -3;

    int num_blocks = (xend - 32 + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for(int y = 3; y < yend; y++)
    {
#if PEELING_ENABLED
        for (int x = 3; x < 32; x++) {
            const uint8_t* p = data + y * row_stride + x;
            if (CVD::is_corner_10<CVD::Less>(p, row_stride, barrier) || CVD::is_corner_10<CVD::Greater>(p, row_stride, barrier))
                corners.push_back(ImageRef(x, y));
        }
#endif

        for (int block_idx = 0; block_idx < num_blocks; block_idx++) {
            int block_start = 32 + block_idx * BLOCK_SIZE;
            int block_end = std::min(xend, block_start + BLOCK_SIZE);

            for (int x = block_start; x < block_end; x += 32)
            {
#if COUNT_CHECKS
                check[0]++;
#endif

                const uint8_t* p = data + y * row_stride + x;

                __m256i lo, hi;
                {
                    // lo = *p - c_barrier < 0 ? 0 : *p - c_barrier;
                    // hi = *p + c_barrier > 255 ? 255 : *p + c_barrier;

                    const __m256i v = _mm256_loadu_si256((const __m256i*)(p));
                    lo = _mm256_subs_epu8(v, barriers);
                    hi = _mm256_adds_epu8(barriers, v);
                }

                uint64_t ans_b, ans_e;
                {
                    // uint8_t top = *(p - stride);
                    // uint8_t bottom = *(p + stride);

                    __m256i top = _mm256_loadu_si256((const __m256i*)(p - stride));
                    __m256i bottom = _mm256_loadu_si256((const __m256i*)(p + stride));

                    CHECK_BARRIER_AXV2(lo, hi, top, ans_b);
                    CHECK_BARRIER_AXV2(lo, hi, bottom, ans_e);

                    if (!(ans_b | ans_e))
                        continue;
                }

#if COUNT_CHECKS
                check[1]++;
#endif
                uint64_t ans_m, ans_p, possible;
                {
                    // uint8_t ul = *(p - 2 - 2 * row_stride);
                    // uint8_t lr = *(p + 2 + 2 * row_stride);

                    __m256i ul = _mm256_loadu_si256((const __m256i*)(p - 2 - 2 * row_stride));
                    __m256i lr = _mm256_loadu_si256((const __m256i*)(p + 2 + 2 * row_stride));

                    CHECK_BARRIER_AXV2(lo, hi, ul, ans_m);
                    CHECK_BARRIER_AXV2(lo, hi, lr, ans_p);

                    possible = (ans_m & ans_b) | (ans_e & ans_p);
                    if (!possible)
                        continue;
                }


#if COUNT_CHECKS
                check[2]++;
#endif
                uint64_t ans_o, ans_n;
                {
                    // uint8_t ll = *(p - 2 + 2 * row_stride);
                    // uint8_t ur = *(p + 2 - 2 * row_stride);

                    __m256i ll = _mm256_loadu_si256((const __m256i*)(p - 2 + 2 * row_stride));
                    __m256i ur = _mm256_loadu_si256((const __m256i*)(p + 2 - 2 * row_stride));

                    CHECK_BARRIER_AXV2(lo, hi, ll, ans_o);
                    CHECK_BARRIER_AXV2(lo, hi, ur, ans_n);

                    possible &= ans_o | (ans_b & ans_n);
                    possible &= ans_n | (ans_e & ans_o);
                    if (!possible)
                        continue;
                }

#if COUNT_CHECKS
                check[3]++;
#endif
                uint64_t ans_h, ans_k;
                {
                    // uint8_t left = *(p - 3);
                    // uint8_t right = *(p + 3);

                    __m256i left = _mm256_loadu_si256((const __m256i*)(p - 3));
                    __m256i right = _mm256_loadu_si256((const __m256i*)(p + 3));

                    CHECK_BARRIER_AXV2(lo, hi, left, ans_h);
                    CHECK_BARRIER_AXV2(lo, hi, right, ans_k);

                    possible &= ans_h | (ans_n & ans_k & ans_p);
                    possible &= ans_k | (ans_m & ans_h & ans_o);
                    if (!possible)
                        continue;
                }

#if COUNT_CHECKS
                check[4]++;
#endif
                uint64_t ans_a, ans_c;
                {
                    // uint8_t a = *(p - 1 - stride);
                    // uint8_t c = *(p + 1 - stride);

                    __m256i a = _mm256_loadu_si256((const __m256i*)(p - 1 - stride));
                    __m256i c = _mm256_loadu_si256((const __m256i*)(p + 1 - stride));

                    CHECK_BARRIER_AXV2(lo, hi, a, ans_a);
                    CHECK_BARRIER_AXV2(lo, hi, c, ans_c);

                    possible &= ans_a | (ans_e & ans_p);
                    possible &= ans_c | (ans_o & ans_e);
                    if (!possible)
                        continue;
                }

#if COUNT_CHECKS
                check[5]++;
#endif

                uint64_t ans_d, ans_f;
                {
                    __m256i d = _mm256_loadu_si256((const __m256i*)(p - 1 + stride));
                    __m256i f = _mm256_loadu_si256((const __m256i*)(p + 1 + stride));

                    //__m256i f = _mm256_insert_epi16(_mm256_srli_si256(d, 2), *(const unsigned short*)(p + 15 + stride), 7);
                    CHECK_BARRIER_AXV2(lo, hi, d, ans_d);
                    CHECK_BARRIER_AXV2(lo, hi, f, ans_f);
                    const uint64_t ans_abc = ans_a & ans_b & ans_c;
                    possible &= ans_d | (ans_abc & ans_n);
                    possible &= ans_f | (ans_m & ans_abc);
                    if (!possible)
                        continue;
                }

#if COUNT_CHECKS
                check[6]++;
#endif
                uint64_t ans_g, ans_i;
                {
                    // uint8_t g = *(p - 3 - row_stride);
                    // uint8_t ii = *(p - 3 + row_stride);

                    __m256i g = _mm256_loadu_si256((const __m256i*)(p - 3 - row_stride));
                    __m256i ii = _mm256_loadu_si256((const __m256i*)(p - 3 + row_stride));

                    CHECK_BARRIER_AXV2(lo, hi, g, ans_g);
                    CHECK_BARRIER_AXV2(lo, hi, ii, ans_i);

                    possible &= ans_g | (ans_f & ans_p & ans_k);
                    possible &= ans_i | (ans_c & ans_n & ans_k);
                    if (!possible)
                        continue;
                }

#if COUNT_CHECKS
                check[7]++;
#endif

                uint64_t ans_j, ans_l;
                {
                    // uint8_t jj = *(p + 3 - row_stride);
                    // uint8_t l = *(p + 3 + row_stride);

                    __m256i jj = _mm256_loadu_si256((const __m256i*)(p + 3 - row_stride));
                    __m256i l = _mm256_loadu_si256((const __m256i*)(p + 3 + row_stride));

                    CHECK_BARRIER_AXV2(lo, hi, jj, ans_j);
                    CHECK_BARRIER_AXV2(lo, hi, l, ans_l);

                    const uint64_t ans_ghi = ans_g & ans_h & ans_i;
                    possible &= ans_j | (ans_d & ans_o & ans_ghi);
                    possible &= ans_l | (ans_m & ans_a & ans_ghi);
                    if (!possible)
                        continue;
                }

#if COUNT_CHECKS
                check[8]++;
#endif

                possible |= (possible >> 32);

                for (uint64_t i = 0; i < 32; i++) {
                    if (possible & ((uint64_t)1 << i)) {
#if COUNT_CHECKS
                        check[9]++;
#endif
                        corners.push_back(ImageRef(x + i, y));
                    }
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
