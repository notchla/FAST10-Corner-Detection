#include "fast.h"
#include "utils.h"

using namespace std;
namespace fs = std::filesystem;

std::vector<Lane> lane_checks;
uint8_t* lane_check;

int main(int argc, char** argv) {
    if (argc < 3) {
        exit(1);
    }

    vector<pair<string, fast_func*>> functions = {
        // {"slow10", fastX_slow}
        {"scalar_10", fast10_scalar},
        {"sse2_10", fast10_sse2},
        {"avx2_10", fast10_avx2},
        {"avx2_blocking_10", fast10_avx2_blocking},
#if AVX512_ENABLED
        {"avx512_10", fast10_avx512},
#endif
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
    uint64_t height = width;
    uint64_t size = width * height;

#if TRAIN_BP
    int xoffset = 0;
    int yoffset = 0;

    ImageRef img_start(xoffset, yoffset);
    ImageRef img_size(width, height);
    CVD::Image<CVD::byte> img = full.sub_image(img_start, img_size);

    uint8_t* data = copy_image_to_worst_case_alignment(img, 64);
    uint32_t row_stride = img.row_stride();

    // Number of repetitions for performance measurements
    double seconds = 10;
    double expected_cycles = (double)size;
    uint64_t count = std::max((uint64_t)3ull, (uint64_t)std::ceil(seconds * 2.4e9 / (double)expected_cycles)) * 9;
    if (name == "sse2_10")
        count *= 16;
    if (name == "avx2_10" || name == "avx2_blocking_10")
        count *= 32;

    std::vector<ImageRef> corners;

    for (int j = 0; j < count; j++)
    {
        func(data, width, height, row_stride, corners, threshold);
        corners.clear();
    }
#else
    uint64_t target_size = 64 * 1024 * 1024;
    uint64_t images_count = max(1ull, (target_size + (width * height - 1)) / (width * height));
    uint32_t row_stride;
    ImageRef img_size(width, height);
;

    // Number of repetitions for performance measurements
    uint64_t repetition_count = 200;
    if (name.find("sse2") != string::npos) {
        repetition_count *= 2;
    }
    else if (name.find("avx") != string::npos) {
        repetition_count *= 3;
    }

    srand(1337);
    rand();

    vector<uint8_t*> images;
    for (int j = 0; j < images_count; j++) {
        int xoffset = img_size.x < 8192 ? rand() % (8192 - img_size.x) : 0;
        int yoffset = img_size.y < 8192 ? rand() % (8192 - img_size.y) : 0;

        ImageRef img_start(xoffset, yoffset);

        CVD::Image<CVD::byte> img = full.sub_image(img_start, img_size);
        uint8_t* data = copy_image_to_worst_case_alignment(img, 64);
        images.push_back(data);

        row_stride = img.row_stride();
    }
    printf("%-20s [%4llu x %4llu] x %4llu = %6.2f MB\n", name.c_str(), width, height, images_count, (double)images_count * width * height / (1024.0 * 1024.0));

    for (int j = 0; j < repetition_count; j++)
    {
        uint64_t total_cycles = 0;
        for (uint8_t* img : images) {
            vector<ImageRef> corners;

#if WARM_CACHE
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x += 64) {
                    warm_cache_counter.fetch_add(img[row_stride * y + x], std::memory_order::memory_order_relaxed);
                }
            }
#endif

            uint64_t begin_cycles = rdtsc();
            func(img, width, height, row_stride, corners, threshold);
            uint64_t cycles = rdtsc() - begin_cycles;
            total_cycles += cycles;
        }
    }
#endif

    return 0;
}
