#include "utils.h"


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


uint8_t* copy_image_aligned(const CVD::Image<CVD::byte>& I, uint32_t alignment) {
    uint64_t width = I.size().x;
    uint64_t height = I.size().y;
    uint64_t stride = width * ((width + alignment - 1) / width);

#ifdef _WIN32
    uint8_t* data = (uint8_t*)_aligned_malloc(stride * height, alignment);
#else
    uint8_t* data = (uint8_t*)aligned_alloc(alignment, stride * height);
#endif

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            *(data + y * stride + x) = I[y][x];
        }
    }

    return data;
}

void aligned_free(void* data) {
#ifdef _WIN32
    _aligned_free(data);
#else
    free(data);
#endif
}
