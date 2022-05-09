#include "fast.h"

#define CONTIG_VALUE 10

void fastX_slow(const CVD::BasicImage<CVD::byte>& I, std::vector<ImageRef>& corners, const int barrier) {
    const int row_stride = I.row_stride();
    const int stride = 3 * row_stride;

    int xend = I.size().x - 3;
    int yend = I.size().y - 3;
    CVD::byte c_barrier = (CVD::byte)barrier;

    int strides[] = { -stride, -stride + 1, -2 * row_stride + 2, -row_stride + 3, 3, row_stride + 3, 2 * row_stride + 2, stride + 1,
                    stride, stride - 1, 2 * row_stride - 2, row_stride - 3, -3, -row_stride - 3, -2 * row_stride - 2, -stride - 1,
                    -stride, -stride + 1, -2 * row_stride + 2, -row_stride + 3, 3, row_stride + 3, 2 * row_stride + 2, stride + 1,
                    stride, stride - 1, 2 * row_stride - 2, row_stride - 3, -3, -row_stride - 3, -2 * row_stride - 2, -stride - 1 };

    for (int y = 3; y < yend; y++) {
        for (int x = 3; x < xend; x++) {

            const CVD::byte* p = &I[y][x];
            CVD::byte lo = *p - c_barrier < 0 ? 0 : *p - c_barrier;
            CVD::byte hi = *p + c_barrier > 255 ? 255 : *p + c_barrier;

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