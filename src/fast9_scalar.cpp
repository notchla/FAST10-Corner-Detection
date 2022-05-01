#include "fast.h"

#define CHECK_BARRIER_SCALAR(lo, hi, other, flags)			\
{													    	\
     CVD::byte diff = lo - other < 0 ? 0 : lo - other;		\
     CVD::byte diff2 = other - hi < 0 ? 0 : other - hi;		\
     diff = diff != 0 ? 0x1 : 0; 					        \
     diff2 = diff2 != 0 ? 0x1 : 0;					        \
     flags = diff | (diff2 << 1);                           \
}

void fast9_scalar(const CVD::BasicImage<CVD::byte>& I, std::vector<ImageRef>& corners, const int barrier) {
    const int row_stride = I.row_stride();
    const int stride = 3 * row_stride;

    int xend = I.size().x - 3;
    int yend = I.size().y - 3;
    CVD::byte c_barrier = (CVD::byte)barrier;

    for (int y = 3; y < yend; y++) {
        for (int x = 3; x < xend; x++) {
#if COUNT_CHECKS
            check[0]++;
#endif

            const CVD::byte* p = &I[y][x];
            CVD::byte lo = *p - c_barrier < 0 ? 0 : *p - c_barrier;
            CVD::byte hi = *p + c_barrier > 255 ? 255 : *p + c_barrier;

            CVD::byte top = *(p - stride);
            CVD::byte bottom = *(p + stride);

            unsigned int ans_0, ans_8, possible;

            CHECK_BARRIER_SCALAR(lo, hi, top, ans_0);
            CHECK_BARRIER_SCALAR(lo, hi, bottom, ans_8);
            possible = ans_0 | ans_8;

            if (!possible)
                continue;
#if COUNT_CHECKS
            check[1]++;
#endif

            unsigned int ans_15, ans_1;
            CVD::byte a = *(p - 1 - stride);
            CVD::byte c = *(p + 1 - stride);
            CHECK_BARRIER_SCALAR(lo, hi, a, ans_15);
            CHECK_BARRIER_SCALAR(lo, hi, c, ans_1);
            possible &= ans_8 | (ans_15 & ans_1);
            if (!possible)
                continue;
#if COUNT_CHECKS
            check[2]++;
#endif

            unsigned int ans_9, ans_7;
            CVD::byte d = *(p + stride - 1);
            CVD::byte f = *(p + stride + 1);
            CHECK_BARRIER_SCALAR(lo, hi, d, ans_9);
            CHECK_BARRIER_SCALAR(lo, hi, f, ans_7);
            possible &= ans_9 | (ans_0 & ans_1);
            possible &= ans_7 | (ans_15 & ans_0);
            if (!possible)
                continue;
#if COUNT_CHECKS
            check[3]++;
#endif

            unsigned int ans_12, ans_4;
            CVD::byte left = *(p - 3);
            CVD::byte right = *(p + 3);
            CHECK_BARRIER_SCALAR(lo, hi, left, ans_12);
            CHECK_BARRIER_SCALAR(lo, hi, right, ans_4);
            possible &= ans_12 | (ans_4 & (ans_1 | ans_7));
            possible &= ans_4 | (ans_12 & (ans_9 | ans_15));
            if (!possible)
                continue;
#if COUNT_CHECKS
            check[4]++;
#endif

            unsigned int ans_14, ans_6;
            CVD::byte ul = *(p - 2 - 2 * row_stride);
            CVD::byte lr = *(p + 2 + 2 * row_stride);
            CHECK_BARRIER_SCALAR(lo, hi, ul, ans_14);
            CHECK_BARRIER_SCALAR(lo, hi, lr, ans_6);

            const unsigned int ans_6_7 = ans_6 & ans_7;
            possible &= ans_14 | (ans_6_7 & (ans_4 | (ans_8 & ans_9)));
            possible &= ans_1 | (ans_6_7) | ans_12;

            const unsigned int ans_14_15 = ans_14 & ans_15;
            possible &= ans_6 | (ans_14_15 & (ans_12 | (ans_0 & ans_1)));
            possible &= ans_9 | (ans_14_15) | ans_4;

            if (!possible)
                continue;
#if COUNT_CHECKS
            check[5]++;
#endif
            

            unsigned int ans_10, ans_2;
            CVD::byte ll = *(p - 2 + 2 * row_stride);
            CVD::byte ur = *(p + 2 - 2 * row_stride);
            CHECK_BARRIER_SCALAR(lo, hi, ll, ans_10);
            CHECK_BARRIER_SCALAR(lo, hi, ur, ans_2);

            const unsigned int ans_1_2 = ans_1 & ans_2;
            possible &= ans_10 | (ans_1_2 & ((ans_0 & ans_15) | ans_4));
            possible &= ans_12 | (ans_1_2) | (ans_6 & ans_7);

            const unsigned int ans_9_10 = ans_9 & ans_10;
            possible &= ans_2 | (ans_9_10 & ((ans_7 & ans_8) | ans_12));
            possible &= ans_4 | (ans_9_10) | (ans_14 & ans_15);

            possible &= ans_8 | ans_14 | ans_2;
            possible &= ans_0 | ans_10 | ans_6;

            if (!possible)
                continue;
#if COUNT_CHECKS
            check[6]++;
#endif
            

            unsigned int ans_13, ans_5;
            CVD::byte g = *(p - 3 - row_stride);
            CVD::byte l = *(p + 3 + row_stride);
            CHECK_BARRIER_SCALAR(lo, hi, g, ans_13);
            CHECK_BARRIER_SCALAR(lo, hi, l, ans_5);

            const unsigned int ans_15_0 = ans_15 & ans_0;
            const unsigned int ans_7_8 = ans_7 & ans_8;

            const unsigned int ans_12_13 = ans_12 & ans_13;
            possible &= ans_5 | (ans_12_13 & ans_14 & ((ans_15_0) | ans_10));
            possible &= ans_7 | (ans_1 & ans_2) | (ans_12_13);
            possible &= ans_2 | (ans_12_13) | (ans_7_8);

            const unsigned int ans_4_5 = ans_4 & ans_5;
            //const unsigned int ans_9_10 = ans_9 & ans_10;
            possible &= ans_13 | (ans_4_5 & ans_6 & ((ans_7_8) | ans_2));
            possible &= ans_15 | (ans_4_5) | (ans_9_10);
            possible &= ans_10 | (ans_4_5) | (ans_15_0);
            possible &= ans_15 | (ans_9_10) | (ans_4_5);

            possible &= ans_8 | (ans_13 & ans_14) | ans_2;
            possible &= ans_0 | (ans_5 & ans_6) | ans_10;

            if (!possible)
                continue;
#if COUNT_CHECKS
            check[7]++;
#endif
            

            unsigned int ans_11, ans_3;
            CVD::byte ii = *(p - 3 + row_stride);
            CVD::byte jj = *(p + 3 - row_stride);
            CHECK_BARRIER_SCALAR(lo, hi, ii, ans_11);
            CHECK_BARRIER_SCALAR(lo, hi, jj, ans_3);

            const unsigned int ans_2_3 = ans_2 & ans_3;
            possible &= ans_11 | (ans_2_3 & ans_4 & ((ans_0 & ans_1) | (ans_5 & ans_6)));
            possible &= ans_13 | (ans_7 & ans_8) | (ans_2_3);
            possible &= ans_8 | (ans_2_3) | (ans_13 & ans_14);

            const unsigned int ans_11_12 = ans_11 & ans_12;
            possible &= ans_3 | (ans_10 & ans_11_12 & ((ans_8 & ans_9) | (ans_13 & ans_14)));
            possible &= ans_1 | (ans_11_12) | (ans_6 & ans_7);
            possible &= ans_6 | (ans_0 & ans_1) | (ans_11_12);

            const unsigned int ans_3_4 = ans_3 & ans_4;
            possible &= ans_9 | (ans_3_4) | (ans_14 & ans_15);
            possible &= ans_14 | (ans_8 & ans_9) | (ans_3_4);

            const unsigned int ans_10_11 = ans_10 & ans_11;
            possible &= ans_5 | (ans_15 & ans_0) | (ans_10_11);
            possible &= ans_0 | (ans_10_11) | (ans_5 & ans_6);

            if (!possible)
                continue;
#if COUNT_CHECKS
            check[8]++;
#endif
            

            possible |= (possible >> 1);

            if (possible)
                corners.push_back(ImageRef(x, y));
        }

    }
}
