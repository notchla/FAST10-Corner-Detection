#include "fast.h"

void libcvd_wrapper(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier, cvd_fast_func* libcvf_f){

    CVD::SubImage<CVD::byte> image(data, CVD::ImageRef(width, height), row_stride);
    CVD::Image<CVD::byte> img(image);

    libcvf_f(img, corners, barrier);
}