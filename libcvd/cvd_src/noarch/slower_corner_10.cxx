#include "cvd_src/fast/prototypes.h"
#include <cvd/fast_corner.h>

namespace CVD
{
void fast_corner_detect_10(const BasicImage<byte>& i, std::vector<ImageRef>& corners, int b)
{
	fast_corner_detect_plain_10(i, corners, b);
}
}
