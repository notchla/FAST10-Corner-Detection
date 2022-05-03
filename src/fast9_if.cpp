#include "fast.h"

#define CHECK_BARRIER_SCALAR(lo, hi, other, flags)			\
{													    	\
     CVD::byte diff = lo - other < 0 ? 0 : lo - other;		\
     CVD::byte diff2 = other - hi < 0 ? 0 : other - hi;		\
     diff = diff != 0 ? 0x1 : 0; 					        \
     diff2 = diff2 != 0 ? 0x1 : 0;					        \
     flags = diff | (diff2 << 1);                           \
}

void fast9_if(const CVD::BasicImage<CVD::byte>& I, std::vector<ImageRef>& corners, const int barrier) {
    const int row_stride = I.row_stride();
    const int stride = 3 * row_stride;

    int xend = I.size().x - 3;
    int yend = I.size().y - 3;
    CVD::byte c_barrier = (CVD::byte)barrier;

    for (int y = 3; y < yend; y++) {
        for (int x = 3; x < xend; x++) {
            if (CVD::is_corner_9<CVD::Less>(&I[y][x], I.row_stride(), barrier) || CVD::is_corner_9<CVD::Greater>(&I[y][x], I.row_stride(), barrier))
                corners.push_back(ImageRef(x, y));
        }
    }
}
