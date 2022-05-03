#include "fast.h"

uint64_t check[9];

using namespace std;
namespace fs = std::filesystem;


typedef pair<string, vector<fs::path>> DataSet;

// Return a list of datasets, each described by a name and a list of paths to data files
vector<DataSet> find_datasets(const fs::path& data_path) 
{
    vector<DataSet> result;

    for (auto& d : fs::directory_iterator(data_path)) 
    {
        if (!d.is_directory()) continue;

        string name = d.path().filename().string();
        fs::path path = (d.path() / "frames");
        if (!fs::is_directory(path)) continue;

        std::vector<fs::path> files;
        for (auto& f : fs::directory_iterator(path)) 
        {
            if(f.is_regular_file())
                files.push_back(fs::path(f).make_preferred());
        }

        result.push_back(make_pair(name, files));
    }

    return result;
}

void run_tests(const vector<DataSet>& dataset, const vector<pair<string, fast_func*>>& functions) 
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
                CVD::fast_corner_detect_9(img, ref, threshold);
                sort(ref.begin(), ref.end());

                for (auto& [name, func] : functions) 
                {
                    // Run corner detection
                    vector<CVD::ImageRef> corners;
                    func(img, corners, threshold);
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
void count_checks(const fs::path& out_path, fast_func* function, CVD::Image<CVD::byte>& img)
{
    

    FILE* outf = std::fopen(out_path.generic_u8string().c_str(), "wb");

    for (int threshold = 0; threshold < 100; threshold += 10) {
        // Zero out counters
        for (int i = 0; i < sizeof(check) / sizeof(check[0]); i++) {
            check[i] = 0;
        }

        vector<ImageRef> corners;
        function(img, corners, threshold);

        fprintf(outf, "%3d ", threshold);
        for (int i = 0; i < sizeof(check) / sizeof(check[0]); i++) {
            fprintf(outf, "%9llu ", check[i]);
        }
        fprintf(outf, "\n");
    }

    fclose(outf);
}

void performance_plot(const fs::path& out_path, const fs::path& image_path, const vector<pair<string, fast_func*>>& functions) {
    CVD::Image<CVD::byte> full;
    CVD::img_load(full, image_path.string());
    assert(full.size().x >= 8192 && full.size().y >= 8192);


    for (int i = 128; i <= 8192; i += 128) 
    {
        uint64_t width = i;
        uint64_t height = i;
        uint64_t size = width * height;

        ImageRef img_start(0, 0);
        ImageRef img_size(width, height);
        CVD::Image<CVD::byte> img = full.sub_image(img_start, img_size);

        // Number of repetitions for performance measurements
        uint64_t expected_cycles = size * 10;
        uint64_t count = std::max(3ull, (uint64_t)std::ceil(1e9 / (double)expected_cycles));

        for (auto& [name, func] : functions)
        {
            stringstream out_name;
            out_name << name << "_" << width << "x" << height;

#if COUNT_CHECKS
            // If COUNT_CHECKS is defined, only output the number of checks
            out_name << "_count.dat";
            count_checks(out_path / out_name.str(), func, img);
            cout << out_name.str() << endl;
#else
            // Otherwise measure and output performance
            out_name << "_cycles.dat";

            cout << name << " [" << width << "x" << height << "] | count:" << count << endl;

            FILE* outf = std::fopen((out_path / out_name.str()).generic_u8string().c_str(), "wb");

            for (int threshold = 0; threshold < 100; threshold += 10) {
                uint64_t total_cycles = 0;
                for (int j = 0; j < count; j++)
                {
                    vector<ImageRef> corners;

                    uint64_t begin_cycles = rdtsc();
                    func(img, corners, threshold);
                    total_cycles += rdtsc() - begin_cycles;
                }

                double cycles = (double)total_cycles / (double)count;

                fprintf(outf, "%3d %16.3f\n", threshold, cycles);
            }

            fclose(outf);
#endif
        }
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
        //{ "scalar", fast9_scalar },
        { "sse2", fast9_sse2 },
    };

    //run_tests(dataset, functions);
    performance_plot(out_dir, "../data/box0_big.png", functions);

    return 0;
}
