#include "fast.h"

void libcvd_sse2(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier) 
{
    CVD::SubImage<CVD::byte> image(data, CVD::ImageRef(width, height), row_stride);
    CVD::fast_corner_detect_10(image, corners, barrier);
}

void libcvd_if(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier) 
{
    CVD::SubImage<CVD::byte> image(data, CVD::ImageRef(width, height), row_stride);
    CVD::fast_corner_detect_plain_10(image, corners, barrier);
}
