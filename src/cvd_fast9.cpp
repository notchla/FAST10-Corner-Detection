#include <cvd/image_io.h>
#include <cvd/fast_corner.h>

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;

namespace CVD {
    void fast_corner_detect_plain_9(const BasicImage<byte>& i, vector<ImageRef>& corners, int b);
}

int main(int argc, char** argv) {
    string data_dir = "../data/";
    string out_dir  = "../output/";
    
    for(auto& d: fs::directory_iterator(data_dir)) {
        string name = d.path().filename();
        string path = d.path() / "frames";

        fs::path out_path = fs::path(out_dir) / name / "fast_corner_detect_9";
        fs::create_directories(out_path);

        for(auto& f: fs::directory_iterator(path)) {

            // Load img from file
            CVD::Image<CVD::byte> img;
            CVD::img_load(img, f.path());

            // Allocate output corners
            vector<CVD::ImageRef> corners;
            int threshold = 64;

            // Run corner detection
            CVD::fast_corner_detect_9(img, corners, threshold);

            // Print results
            cout << f.path() << img.size() << ": " << endl;
            cout << "fast_corner_detect_9:       " << corners.size() << " corners out of " << img.size().x * img.size().y << endl;  
            cout << endl;

            // Sort corners
            sort(corners.begin(), corners.end());

            // Output
            ofstream o;
            o.open(out_path / (string(f.path().stem()) + ".txt"), fstream::trunc);

            for(auto& c: corners) {
                o << c << endl;
            }
        }
    }
}
