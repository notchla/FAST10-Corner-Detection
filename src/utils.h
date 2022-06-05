#pragma once

#include "fast.h"

using namespace std;
namespace fs = std::filesystem;

typedef pair<string, vector<fs::path>> DataSet;

vector<DataSet> find_datasets(const fs::path& data_path);
uint8_t* copy_image_aligned(const CVD::Image<CVD::byte>& I, uint32_t alignment);
void aligned_free(void* data);
