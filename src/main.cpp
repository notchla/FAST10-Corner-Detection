#include "fast.h"
#include "utils.h"

uint64_t check[10];
std::vector<Lane> lane_checks;
uint8_t* lane_check;

using namespace std;
namespace fs = std::filesystem;

void run_tests(const vector<DataSet>& dataset, cvd_fast_func* reference,  vector<pair<string, fast_func*>>& functions) 
{
    for (auto& [name, paths] : dataset) 
    {
        cout << name << endl;
        for (auto& p : paths) 
        {
            // Load img from file
            CVD::Image<CVD::byte> img;
            CVD::img_load(img, p.string());

            // Total number of pixels for which we run the corner detection on this image
            uint64_t pixels = ((uint64_t)img.size().x - 3) * ((uint64_t)img.size().y - 3);

            // Test 
            for (int threshold = 0; threshold < 130; threshold += 10)
            {
                // Run reference corner detection
                vector<CVD::ImageRef> ref;
                reference(img, ref, threshold);
                sort(ref.begin(), ref.end());

                for (auto& [name, func] : functions) 
                {
                    // Run corner detection
                    vector<CVD::ImageRef> corners;
                    func(img.data(), img.size().x, img.size().y, img.row_stride(), corners, threshold);
                    sort(corners.begin(), corners.end());

                    if (!(ref == corners)) 
                    {
                        cout << "\tError - " << p.string() << " | " << name << "Detected: " << corners.size()
                            << ", Expected: " << ref.size() << endl;
                        assert(false);
                        exit(1);
                    }
                }
            }

            cout << "\tOK - " << p << endl;
        }
    }
}

// Count checks on a given image and output data to 'out_path'
void count_checks(FILE* outf, fast_func* function, CVD::Image<CVD::byte>& img, int threshold)
{
    //for (int threshold = 0; threshold < 50; threshold += 5) 
    {
        // Zero out counters
        for (int i = 0; i < sizeof(check) / sizeof(check[0]); i++) {
            check[i] = 0;
        }

        vector<ImageRef> corners;
        function(img.data(), img.size().x, img.size().y, img.row_stride(), corners, threshold);

        std::fprintf(outf, "%4d %4d ", img.size().x, img.size().y);
        for (int i = 0; i < sizeof(check) / sizeof(check[0]); i++) {
            std::fprintf(outf, "%9llu ", check[i]);
        }
        std::fprintf(outf, "\n");
    }
}

// Gets the position and number of checks for each lane on a given image and output data to 'out_path'
void count_lane_checks(const fs::path& out_path, const fs::path& image_path, const vector<pair<string, fast_func*>>& functions) {
#if COUNT_CHECK_POSITION
    CVD::Image<CVD::byte> full;
    CVD::img_load(full, image_path.string());
    assert(full.size().x >= 8192 && full.size().y >= 8192);
    int threshold = 25;
    int xoffset = 0;
    int yoffset = 0;

    for (auto& [name, func] : functions)
    {
        stringstream out_name;

        out_name << name << "_" << threshold << "_checkposition.dat";

        FILE* outf = std::fopen((out_path / out_name.str()).generic_u8string().c_str(), "wb");

        uint64_t width = 512;
        uint64_t height = 512;
        uint64_t size = width * height;

        ImageRef img_start(xoffset, yoffset);
        ImageRef img_size(width, height);
        CVD::Image<CVD::byte> img = full.sub_image(img_start, img_size);

        if (name == "scalar_10")
            lane_check = (uint8_t*)calloc(size, sizeof(uint8_t));
        else
            lane_checks.clear();

        vector<ImageRef> corners;
        func(img.data(), img.size().x, img.size().y, img.row_stride(), corners, threshold);

        std::fprintf(outf, "%d %d %llu\n", img.size().x, img.size().y, corners.size());
        for (auto& corner : corners) {
            std::fprintf(outf, "%4d %4d\n", corner.y, corner.x);
        }

        if (name == "scalar_10") {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    std::fprintf(outf, "%1d ", *(lane_check + i * width + j));
                }
                std::fprintf(outf, "\n");
            }
            free(lane_check);
        }
        else {
            for (auto& l : lane_checks) {
                std::fprintf(outf, "%4d %4d %3d %d\n", l.y, l.x, l.width, l.checks);
            }
        }

        std::fclose(outf);
    }
#endif
}

void count_dataset_checks(const fs::path& out_path, vector<DataSet>& datasets) {
#if COUNT_CHECK_POSITION
    int threshold = 25;

    for(auto& [name, paths]: datasets) {
        stringstream out_name;
        out_name << name << "_checks.dat";

        FILE* outf = std::fopen((out_path / out_name.str()).generic_u8string().c_str(), "wb");
        std::fprintf(outf, "%d\n", (int)paths.size());

        for (auto& p : paths) {
            printf("%ls\n", p.c_str());
            CVD::Image<CVD::byte> img;
            CVD::img_load(img, p.string());

            // Total number of pixels for which we run the corner detection on this image
            uint64_t pixels = ((uint64_t)img.size().x - 3) * ((uint64_t)img.size().y - 3);

            // Alloc space for lane checks
            lane_check = (uint8_t*)calloc(img.size().x * img.size().y, sizeof(uint8_t));

            // Count checks for each pixel
            vector<ImageRef> corners;
            fast10_scalar(img.data(), img.size().x, img.size().y, img.row_stride(), corners, threshold);

            // output checks
            std::fprintf(outf, "%d %d\n", img.size().x, img.size().y);
            for (int i = 0; i < img.size().y; i++) {
                for (int j = 0; j < img.size().x; j++) {
                    std::fprintf(outf, "%1d ", lane_check[i * img.size().x + j]);
                }
                std::fprintf(outf, "\n");
            }
            std::fprintf(outf, "\n");

            // Free lane checks
            free(lane_check);
        }

        std::fclose(outf);
    }
#endif
}


atomic_uint64_t warm_cache_counter;

void randomized_performance_plot(const fs::path& out_path, const fs::path& image_path, const vector<pair<string, fast_func*>>& functions) {
    CVD::Image<CVD::byte> full;
    CVD::img_load(full, image_path.string());
    assert(full.size().x >= 8192 && full.size().y >= 8192);


    int threshold = 25;
    for (auto& [name, func] : functions)
    {
        stringstream out_name;
        out_name << name << "_" << threshold;

#if COUNT_CHECKS
        out_name << "_count.dat";
#else
        out_name << "_cycles.dat";
#endif

        FILE* outf = std::fopen((out_path / out_name.str()).generic_u8string().c_str(), "wb");

        for (int i = 128; i <= 8192; i += 128)
        //for (int i = 8192; i <= 8192; i += 32)
        {
            uint64_t width = i;
            uint64_t height = i;
            uint64_t size = width * height;
            uint32_t row_stride;

            ImageRef img_size(width, height);

            // Number of different images to test to avoid training the branch predictor and not fitting the measurements to a single image
            uint64_t target_size = 32 * 1024 * 1024;
            uint64_t images_count = max(1ull, (target_size + (width * height - 1)) / (width * height));

            // Number of repetitions for performance measurements
            uint64_t repetition_count = 5;
            if (name.find("sse2") != string::npos) {
                repetition_count = 10;
            }
            else if (name.find("avx") != string::npos) {
                repetition_count = 15;
            }

            srand(1337);
            rand();

            printf("%-20s [%4llu x %4llu] x %4llu = %6.2f MB\n", name.c_str(), width, height, images_count, (double)images_count * width * height / (1024.0 * 1024.0));
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

#if COUNT_CHECKS
            // If COUNT_CHECKS is defined, only output the number of checks

            // Zero out counters
            for (int i = 0; i < sizeof(check) / sizeof(check[0]); i++) {
                check[i] = 0;
            }

            // Measure all images
            for (uint8_t* img : images) {
                vector<ImageRef> corners;
                func(img, width, height, row_stride, corners, threshold);
            }

            // Print checks
            std::fprintf(outf, "%4d %4d ", (int)width, (int)height);
            for (int i = 0; i < sizeof(check) / sizeof(check[0]); i++) {
                std::fprintf(outf, "%9llu ", check[i]);
            }
            std::fprintf(outf, "\n");
#else

            // Otherwise measure and output performance

            vector<uint64_t> measurements;
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
                measurements.push_back(total_cycles);
            }

            std::sort(measurements.begin(), measurements.end());

            uint64_t cycles = (double)measurements[measurements.size() / 2];

            fprintf(outf, "%16llu\n", cycles);

#endif
            for (uint8_t* img : images) {
                aligned_free(img);
            }
        }

        std::fclose(outf);
    }
}

void performance_plot(const fs::path& out_path, const fs::path& image_path, const vector<pair<string, fast_func*>>& functions) {
    CVD::Image<CVD::byte> full;
    CVD::img_load(full, image_path.string());
    assert(full.size().x >= 8192 && full.size().y >= 8192);


    int threshold = 25;
    int xoffset = 0;
    int yoffset = 0;

    for (auto& [name, func] : functions)
    {
        stringstream out_name;
        out_name << name << "_" << threshold;

#if COUNT_CHECKS
        out_name << "_count.dat";
#else
        out_name << "_cycles.dat";
#endif

        FILE* outf = std::fopen((out_path / out_name.str()).generic_u8string().c_str(), "wb");

        //for (int i = 128; i <= 1024; i += 32)
        for (int i = 8192; i <= 8192; i += 32)
        {
            uint64_t width = i;
            uint64_t height = i;
            uint64_t size = width * height;

            ImageRef img_start(xoffset, yoffset);
            ImageRef img_size(width, height);
            CVD::Image<CVD::byte> img = full.sub_image(img_start, img_size);
            
            uint8_t* data = copy_image_to_worst_case_alignment(img, 64);
            uint32_t row_stride = img.row_stride();

#if COUNT_CHECKS
            // If COUNT_CHECKS is defined, only output the number of checks
            count_checks(outf, func, img, threshold);
#else
            // Number of repetitions for performance measurements
            uint64_t expected_cycles = (double)size * 10;
            uint64_t count = std::max((uint64_t)3ull, (uint64_t)std::ceil(1e9 / (double)expected_cycles));
            if (name == "sse2_10")
                count *= 2;
            if (name == "avx2_10" || name == "avx2_10_vecpeeling" || name == "avx2_10_vecpeeling_mask" || name == "avx2_10_unrolled_2" || name == "avx2_10_unrolled_3")
                count *= 3;

            // Otherwise measure and output performance
            cout << name << " [" << width << "x" << height << "] | count:" << count << endl;

            uint64_t total_cycles = 0;

            vector<uint64_t> measurements;
            for (int j = 0; j < count; j++)
            {
                vector<ImageRef> corners;

                uint64_t begin_cycles = rdtsc();
                func(data, width, height, row_stride, corners, threshold);
                uint64_t cycles = rdtsc() - begin_cycles;

                measurements.push_back(cycles);
            }

            std::sort(measurements.begin(), measurements.end());

            uint64_t cycles = (double)measurements[measurements.size() / 2];

            fprintf(outf, "%16llu\n", cycles);

            aligned_free(data);
#endif
        }

        std::fclose(outf);
    }
}

int main(int argc, char** argv) {
    string current_dir = fs::current_path().string();
    string data_dir = "../data/";
    string out_dir = "../output";

    fs::create_directories(out_dir);

    if (argc > 1) {
        data_dir = argv[1];
    }

    auto dataset = find_datasets(data_dir);
    vector<pair<string, fast_func*>> functions = {
        // { "scalar", fast9_scalar },
        // { "if", fast9_if },
        // { "sse2", fast9_sse2 },
        {"scalar_10", fast10_scalar}
        //{"sse2_10", fast10_sse2},
        //{"avx2_10", fast10_avx2},
        //{"avx512_10", fast10_avx512}
        // {"slow10", fastX_slow}
        // {"scalar_10_block", fast10_scalar_block},
        // {"avx2_10_unrolled_3", fast10_avx2_unrolled_3},
        // {"avx2_10_unrolled_2", fast10_avx2_unrolled_2},
        // {"avx2_10_vecpeeling", fast10_avx2_vecpeeling},
        // {"avx2_10_vecpeeling_mask", fast10_avx2_vecpeeling_mask}
        // {"avx2_10_checkposition", fast10_avx2_checkposition}
    };

    //run_tests(dataset, CVD::fast_corner_detect_10, functions);
    //count_lane_checks(out_dir, "../data/box0_big.png", functions);
    count_dataset_checks(out_dir, dataset);
#if TRAIN_BP
    //performance_plot(out_dir, "../data/box0_big.png", functions);
#else
    //randomized_performance_plot(out_dir, "../data/box0_big.png", functions);
#endif


    return 0;
}
