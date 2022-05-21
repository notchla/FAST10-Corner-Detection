#include "fast.h"
#include "utils.h"

uint64_t check[9];

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

void performance_plot(const fs::path& out_path, const fs::path& image_path, const vector<pair<string, fast_func*>>& functions) {
    CVD::Image<CVD::byte> full;
    CVD::img_load(full, image_path.string());
    assert(full.size().x >= 8192 && full.size().y >= 8192);


    int threshold = 25;
    int xoffset = 256;
    int yoffset = 256;

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

        for (int i = 128; i <= 1024; i += 32)
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
            if (name == "avx2_10")
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
             {"scalar_10", fast10_scalar},
        {"sse2_10", fast10_sse2},
        {"avx2_10", fast10_avx2},
        {"avx512_10", fast10_avx512}
        // {"slow10", fastX_slow}
    };

    run_tests(dataset, CVD::fast_corner_detect_10, functions);
    //performance_plot(out_dir, "../data/box0_big.png", functions);

    return 0;
}
