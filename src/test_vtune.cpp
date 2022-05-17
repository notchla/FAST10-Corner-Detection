#include "fast.h"
#include "utils.h"

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char** argv) {
    if (argc < 3) {
        exit(1);
    }

    vector<pair<string, fast_func*>> functions = {
        // {"slow10", fastX_slow}
        // {"scalar_10", fast10_scalar},
        {"sse2_10", fast10_sse2},
        {"avx2_10", fast10_avx2},
    };

    string name;
    fast_func* func = 0;
    for (auto& [n, f] : functions)
    {
        if (n == argv[1]) {
            func = f;
            break;
        }
    }
    
    if (!func) {
        exit(1);
    }

    uint64_t width = std::atoi(argv[2]);
    if (width == 0) {
        exit(1);
    }

    CVD::Image<CVD::byte> full;
    CVD::img_load(full, "../data/box0_big.png");
    assert(full.size().x >= 8192 && full.size().y >= 8192);

    int threshold = 25;
    int xoffset = 0;
    int yoffset = 0;

    uint64_t height = width;
    uint64_t size = width * height;

    ImageRef img_start(xoffset, yoffset);
    ImageRef img_size(width, height);
    CVD::Image<CVD::byte> img = full.sub_image(img_start, img_size);

    uint8_t* data = copy_image_to_worst_case_alignment(img, 64);
    uint32_t row_stride = img.row_stride();

    // Number of repetitions for performance measurements
    double seconds = 10;
    double expected_cycles = (double)size;
    uint64_t count = std::max((uint64_t)3ull, (uint64_t)std::ceil(seconds * 2.4e9 / (double)expected_cycles));
    if (name == "sse2_10")
        count *= 16;
    if (name == "avx2_10")
        count *= 32;

    std::vector<ImageRef> corners;
    for (int j = 0; j < count; j++)
    {
        func(data, width, height, row_stride, corners, threshold);
        corners.clear();
    }

    // aligned_free(data);

    return 0;
}
