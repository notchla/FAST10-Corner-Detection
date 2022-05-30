#include "fast.h"

#define CONTIG_VALUE 10

void fastX_slow(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier) {
    int stride = 3 * row_stride;

    uint8_t c_barrier = (uint8_t)barrier;

    int xend = width - 3;
    int yend = height - 3;

    int rstride = row_stride;
    int strides[] = { -stride, -stride + 1, -2 * rstride + 2, -rstride + 3, 3, rstride + 3, 2 * rstride + 2, stride + 1,
                    stride, stride - 1, 2 * rstride - 2, rstride - 3, -3, -rstride - 3, -2 * rstride - 2, -stride - 1,
                    -stride, -stride + 1, -2 * rstride + 2, -rstride + 3, 3, rstride + 3, 2 * rstride + 2, stride + 1,
                    stride, stride - 1, 2 * rstride - 2, rstride - 3, -3, -rstride - 3, -2 * rstride - 2, -stride - 1 };

    for (int y = 3; y < yend; y++) {
        for (int x = 3; x < xend; x++) {

            const uint8_t* p = data + y * row_stride + x;
            uint8_t lo = *p - c_barrier < 0 ? 0 : *p - c_barrier;
            uint8_t hi = *p + c_barrier > 255 ? 255 : *p + c_barrier;

#if CONTIG_VALUE >= 12
            int lowers = 0, highers = 0;
            if (*(p - stride) > hi) highers++; else if (*(p - stride) < lo) lowers++;
            if (*(p + stride) > hi) highers++; else if (*(p + stride) < lo) lowers++;
            if (*(p + 3) > hi) highers++; else if (*(p + 3) < lo) lowers++;
            if (*(p - 3) > hi) highers++; else if (*(p - 3) < lo) lowers++;
            if (lowers < 3 && highers < 3)
                continue;
#endif

            int num_contig = 0;
            int i = 0;
            int mode = 0; // 0 means nothing, 1 means higher, 2 means lower
            while (num_contig < CONTIG_VALUE && i < (16 + CONTIG_VALUE - 1)) {
                const CVD::byte* p_aux = p + strides[i];
                if (*p_aux > hi) {
                    if (mode == 1) {
                        num_contig++;
                    }
                    else {
                        num_contig = 1;
                        mode = 1;
                    }
                }
                else if (*p_aux < lo) {
                    if (mode == 2) {
                        num_contig++;
                    }
                    else {
                        num_contig = 1;
                        mode = 2;
                    }
                }
                else {
                    num_contig = 0;
                    mode = 0;
                }
                i++;
            }

            if (num_contig >= CONTIG_VALUE) {
                corners.push_back(ImageRef(x, y));
            }
        }
    }
}