#include <cvd/image_io.h>
#include <cvd/fast_corner.h>
#include <cvd_src/faster_corner_utilities.h>

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <algorithm>

#ifdef WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#define COUNT_CHECKS true

#if COUNT_CHECKS
uint64_t check[10];
#endif

using namespace std;
namespace fs = std::filesystem;
using CVD::ImageRef;

namespace CVD {
    void fast_corner_detect_plain_9(const BasicImage<byte>& i, vector<ImageRef>& corners, int b);
    #include <cvd_src/corner_9.h>
}

uint64_t rdtsc() {
    return __rdtsc();
}

//probably flags can be done better
#define CHECK_BARRIER_SCALAR(lo, hi, other, flags)  \
	{												\
	 CVD::byte diff = lo - other < 0 ? 0 : lo - other;					\
	 CVD::byte diff2 = other - hi < 0 ? 0 : other - hi;					\
	 diff = diff == 0 ? 0x1 : 0; 					\
	 diff2 = diff2 == 0 ? 0x1 : 0;					\
	 flags = ~(diff | diff2 << 1);       \
	}

void unrolled_faster(const CVD::BasicImage<CVD::byte>& I, std::vector<ImageRef>& corners, const int barrier){
	const int row_stride = I.row_stride(); //remove()
	const int stride = 3 * row_stride;
	
	int xend = I.size().x - 3;
	int yend = I.size().y - 3;
	CVD::byte c_barrier = (CVD::byte) barrier;
	// xend -= (I.size().x - 3) % 16;

	for(int y = 3; y < yend; y++){
		for(int x = 3; x < xend; x++){
			const CVD::byte* p = &I[y][x];
			CVD::byte lo = *p - c_barrier < 0 ? 0 : *p - c_barrier;
			CVD::byte hi = *p + c_barrier > 255 ? 255 : *p + c_barrier;

			CVD::byte top = *(p - stride);
			CVD::byte bottom = *(p + stride);

			unsigned int ans_0, ans_8, possible;

			CHECK_BARRIER_SCALAR(lo, hi, top, ans_0);
			CHECK_BARRIER_SCALAR(lo, hi, bottom, ans_8);
			possible = ans_0 | ans_8;

			if(!possible)
				continue;

			unsigned int ans_15, ans_1;
			CVD::byte a = *(p - 1 - stride);
			CVD::byte c = *(p + 1 - stride);
			CHECK_BARRIER_SCALAR(lo, hi, a, ans_15);
			CHECK_BARRIER_SCALAR(lo, hi, c, ans_1);
			possible &= ans_8 | (ans_15 & ans_1);
			if(!possible)
				continue;

			unsigned int ans_9, ans_7;
			CVD::byte d = *(p + stride - 1);
			CVD::byte f = *(p + stride + 1);
			CHECK_BARRIER_SCALAR(lo, hi, d, ans_9);
			CHECK_BARRIER_SCALAR(lo, hi, f, ans_7);
			possible &= ans_9 | (ans_0 & ans_1);
			possible &= ans_7 | (ans_15 & ans_0);
			if (!possible) {
					continue;
				}

			unsigned int ans_12, ans_4;
			CVD::byte left = *(p - 3);
			CVD::byte right = *(p + 3);
			CHECK_BARRIER_SCALAR(lo, hi, left, ans_12);
			CHECK_BARRIER_SCALAR(lo, hi, right, ans_4);
			possible &= ans_12 | (ans_4 & (ans_1 | ans_7));
			possible &= ans_4 | (ans_12 & (ans_9 | ans_15));
			if (!possible) {
					continue;
				}

			unsigned int ans_14, ans_6;
			CVD::byte ul = *(p - 2 - 2*row_stride); //todo optimize
			CVD::byte lr = *(p + 2 + 2*row_stride); //todo optimize
			CHECK_BARRIER_SCALAR(lo, hi, ul, ans_14);
			CHECK_BARRIER_SCALAR(lo, hi, lr, ans_6);

			const unsigned int ans_6_7 = ans_6 & ans_7;
			possible &= ans_14 | (ans_6_7 & (ans_4 | (ans_8 & ans_9)));
			possible &= ans_1 | (ans_6_7) | ans_12;

			const unsigned int ans_14_15 = ans_14 & ans_15;
			possible &= ans_6 | (ans_14_15 & (ans_12 | (ans_0 & ans_1)));
			possible &= ans_9 | (ans_14_15) | ans_4;

			if (!possible) {
				continue;
			}

			unsigned int ans_10, ans_2;
			CVD::byte ll = *(p - 2 + 2*row_stride); //todo optimize
			CVD::byte ur = *(p + 2 - 2*row_stride); //todo optimize
			CHECK_BARRIER_SCALAR(lo, hi, ll, ans_10);
			CHECK_BARRIER_SCALAR(lo, hi, ur, ans_2);

			const unsigned int ans_1_2 = ans_1 & ans_2;
			possible &= ans_10 | (ans_1_2 & ((ans_0 & ans_15) | ans_4));
			possible &= ans_12 | (ans_1_2) | (ans_6 & ans_7);

			const unsigned int ans_9_10 = ans_9 & ans_10;
			possible &= ans_2 | (ans_9_10 & ((ans_7 & ans_8) | ans_12));
			possible &= ans_4 | (ans_9_10) | (ans_14 & ans_15);

			possible &= ans_8 | ans_14 | ans_2;
			possible &= ans_0 | ans_10 | ans_6;

			if (!possible) {
				continue;
			}

			unsigned int ans_13, ans_5;
			CVD::byte g = *(p - 3  - row_stride);
			CVD::byte l = *(p + 3 + row_stride);
			CHECK_BARRIER_SCALAR(lo, hi, g, ans_13);
			CHECK_BARRIER_SCALAR(lo, hi, l, ans_5);

			const unsigned int ans_15_0 = ans_15 & ans_0;
			const unsigned int ans_7_8 = ans_7 & ans_8;

			const unsigned int ans_12_13 = ans_12 & ans_13;
			possible &= ans_5 | (ans_12_13 & ans_14 & ((ans_15_0) | ans_10));
			possible &= ans_7 | (ans_1 & ans_2) | (ans_12_13);
			possible &= ans_2 | (ans_12_13) | (ans_7_8);

			const unsigned int ans_4_5 = ans_4 & ans_5;
			//const unsigned int ans_9_10 = ans_9 & ans_10;
			possible &= ans_13 | (ans_4_5 & ans_6 & ((ans_7_8) | ans_2));
			possible &= ans_15 | (ans_4_5) | (ans_9_10);
			possible &= ans_10 | (ans_4_5) | (ans_15_0);
			possible &= ans_15 | (ans_9_10) | (ans_4_5);

			possible &= ans_8 | (ans_13 & ans_14) | ans_2;
			possible &= ans_0 | (ans_5 & ans_6) | ans_10;

			if (!possible) {
				continue;
			}

			unsigned int ans_11, ans_3;
			CVD::byte ii = *(p - 3  + row_stride); //todo optimize
			CVD::byte jj = *(p + 3 - row_stride); //todo optimize
			CHECK_BARRIER_SCALAR(lo, hi, ii, ans_11);
			CHECK_BARRIER_SCALAR(lo, hi, jj, ans_3);

			const unsigned int ans_2_3 = ans_2 & ans_3;
			possible &= ans_11 | (ans_2_3 & ans_4 & ((ans_0 & ans_1) | (ans_5 & ans_6)));
			possible &= ans_13 | (ans_7 & ans_8) | (ans_2_3);
			possible &= ans_8 | (ans_2_3) | (ans_13 & ans_14);

			const unsigned int ans_11_12 = ans_11 & ans_12;
			possible &= ans_3 | (ans_10 & ans_11_12 & ((ans_8 & ans_9) | (ans_13 & ans_14)));
			possible &= ans_1 | (ans_11_12) | (ans_6 & ans_7);
			possible &= ans_6 | (ans_0 & ans_1) | (ans_11_12);

			const unsigned int ans_3_4 = ans_3 & ans_4;
			possible &= ans_9 | (ans_3_4) | (ans_14 & ans_15);
			possible &= ans_14 | (ans_8 & ans_9) | (ans_3_4);

			const unsigned int ans_10_11 = ans_10 & ans_11;
			possible &= ans_5 | (ans_15 & ans_0) | (ans_10_11);
			possible &= ans_0 | (ans_10_11) | (ans_5 & ans_6);

			if (!possible) {
				continue;
			}

			possible |= (possible >> 1);

			if(possible)
				corners.push_back(ImageRef(x, y));
		}
		
	}
}

void our_faster_corner_detect_9(const CVD::BasicImage<CVD::byte>& I, std::vector<ImageRef>& corners, const int barrier)
{
	const int row_stride = I.row_stride();
	const int stride = 3 * row_stride;

	const __m128i barriers = _mm_set1_epi8((CVD::byte)barrier);

	int xend = I.size().x - 3;
	xend -= (I.size().x - 3) % 16;

	for(int y = 3; y < I.size().y - 3; y++)
	{
		for(int x = 3; x < 16; x++)
			if(CVD::is_corner_9<CVD::Less>(&I[y][x], I.row_stride(), barrier) || CVD::is_corner_9<CVD::Greater>(&I[y][x], I.row_stride(), barrier))
				corners.push_back(ImageRef(x, y));

		for(int x = 16; x < xend; x += 16)
		{
#if COUNT_CHECKS
            check[0]++;
#endif

			const CVD::byte* p = &I[y][x];
			__m128i lo, hi;
			{
				const __m128i here = _mm_load_si128((const __m128i*)(p));
				lo = _mm_subs_epu8(here, barriers);
				hi = _mm_adds_epu8(barriers, here);
			}
			unsigned int ans_0, ans_8, possible;
			{
				__m128i top = _mm_load_si128((const __m128i*)(p - stride));
				__m128i bottom = _mm_load_si128((const __m128i*)(p + stride));

				CHECK_BARRIER(lo, hi, top, ans_0);
				CHECK_BARRIER(lo, hi, bottom, ans_8);
				possible = ans_0 | ans_8;
				if (!possible) {
					continue;
				}
			}
#if COUNT_CHECKS
            check[1]++;
#endif

			unsigned int ans_15, ans_1;
			{
				__m128i a = _mm_loadu_si128((const __m128i*)(p - 1 - stride));
				__m128i c = _mm_insert_epi16(_mm_srli_si128(a, 2), *(const unsigned short*)(p + 15 - stride), 7);
				CHECK_BARRIER(lo, hi, a, ans_15);
				CHECK_BARRIER(lo, hi, c, ans_1);
				possible &= ans_8 | (ans_15 & ans_1);
				if (!possible) {
					continue;
				}
			}
#if COUNT_CHECKS
            check[2]++;
#endif

			unsigned int ans_9, ans_7;
			{
				__m128i d = _mm_loadu_si128((const __m128i*)(p - 1 + stride));
				__m128i f = _mm_insert_epi16(_mm_srli_si128(d, 2), *(const unsigned short*)(p + 15 + stride), 7);
				CHECK_BARRIER(lo, hi, d, ans_9);
				CHECK_BARRIER(lo, hi, f, ans_7);
				possible &= ans_9 | (ans_0 & ans_1);
				possible &= ans_7 | (ans_15 & ans_0);
				if (!possible) {
					continue;
				}
			}
#if COUNT_CHECKS
            check[3]++;
#endif

			unsigned int ans_12, ans_4;
			{
				__m128i left = _mm_loadu_si128((const __m128i*)(p - 3));
				__m128i right = _mm_loadu_si128((const __m128i*)(p + 3));
				CHECK_BARRIER(lo, hi, left, ans_12);
				CHECK_BARRIER(lo, hi, right, ans_4);
				possible &= ans_12 | (ans_4 & (ans_1 | ans_7));
				possible &= ans_4 | (ans_12 & (ans_9 | ans_15));
				if (!possible) {
					continue;
				}
			}
#if COUNT_CHECKS
            check[4]++;
#endif

			unsigned int ans_14, ans_6;
			{
				__m128i ul = _mm_loadu_si128((const __m128i*)(p - 2 - 2 * row_stride));
				__m128i lr = _mm_loadu_si128((const __m128i*)(p + 2 + 2 * row_stride));
				CHECK_BARRIER(lo, hi, ul, ans_14);
				CHECK_BARRIER(lo, hi, lr, ans_6);
				{
					const unsigned int ans_6_7 = ans_6 & ans_7;
					possible &= ans_14 | (ans_6_7 & (ans_4 | (ans_8 & ans_9)));
					possible &= ans_1 | (ans_6_7) | ans_12;
				}
				{
					const unsigned int ans_14_15 = ans_14 & ans_15;
					possible &= ans_6 | (ans_14_15 & (ans_12 | (ans_0 & ans_1)));
					possible &= ans_9 | (ans_14_15) | ans_4;
				}
				if (!possible) {
					continue;
				}
			}
#if COUNT_CHECKS
            check[5]++;
#endif
			unsigned int ans_10, ans_2;
			{
				__m128i ll = _mm_loadu_si128((const __m128i*)(p - 2 + 2 * row_stride));
				__m128i ur = _mm_loadu_si128((const __m128i*)(p + 2 - 2 * row_stride));
				CHECK_BARRIER(lo, hi, ll, ans_10);
				CHECK_BARRIER(lo, hi, ur, ans_2);
				{
					const unsigned int ans_1_2 = ans_1 & ans_2;
					possible &= ans_10 | (ans_1_2 & ((ans_0 & ans_15) | ans_4));
					possible &= ans_12 | (ans_1_2) | (ans_6 & ans_7);
				}
				{
					const unsigned int ans_9_10 = ans_9 & ans_10;
					possible &= ans_2 | (ans_9_10 & ((ans_7 & ans_8) | ans_12));
					possible &= ans_4 | (ans_9_10) | (ans_14 & ans_15);
				}
				possible &= ans_8 | ans_14 | ans_2;
				possible &= ans_0 | ans_10 | ans_6;
				if (!possible) {
					continue;
				}
			}
#if COUNT_CHECKS
            check[6]++;
#endif

			unsigned int ans_13, ans_5;
			{
				__m128i g = _mm_loadu_si128((const __m128i*)(p - 3 - row_stride));
				__m128i l = _mm_loadu_si128((const __m128i*)(p + 3 + row_stride));
				CHECK_BARRIER(lo, hi, g, ans_13);
				CHECK_BARRIER(lo, hi, l, ans_5);
				const unsigned int ans_15_0 = ans_15 & ans_0;
				const unsigned int ans_7_8 = ans_7 & ans_8;
				{
					const unsigned int ans_12_13 = ans_12 & ans_13;
					possible &= ans_5 | (ans_12_13 & ans_14 & ((ans_15_0) | ans_10));
					possible &= ans_7 | (ans_1 & ans_2) | (ans_12_13);
					possible &= ans_2 | (ans_12_13) | (ans_7_8);
				}
				{
					const unsigned int ans_4_5 = ans_4 & ans_5;
					const unsigned int ans_9_10 = ans_9 & ans_10;
					possible &= ans_13 | (ans_4_5 & ans_6 & ((ans_7_8) | ans_2));
					possible &= ans_15 | (ans_4_5) | (ans_9_10);
					possible &= ans_10 | (ans_4_5) | (ans_15_0);
					possible &= ans_15 | (ans_9_10) | (ans_4_5);
				}

				possible &= ans_8 | (ans_13 & ans_14) | ans_2;
				possible &= ans_0 | (ans_5 & ans_6) | ans_10;
				if (!possible) {
					continue;
				}
			}
#if COUNT_CHECKS
            check[7]++;
#endif

			unsigned int ans_11, ans_3;
			{
				__m128i ii = _mm_loadu_si128((const __m128i*)(p - 3 + row_stride));
				__m128i jj = _mm_loadu_si128((const __m128i*)(p + 3 - row_stride));
				CHECK_BARRIER(lo, hi, ii, ans_11);
				CHECK_BARRIER(lo, hi, jj, ans_3);
				{
					const unsigned int ans_2_3 = ans_2 & ans_3;
					possible &= ans_11 | (ans_2_3 & ans_4 & ((ans_0 & ans_1) | (ans_5 & ans_6)));
					possible &= ans_13 | (ans_7 & ans_8) | (ans_2_3);
					possible &= ans_8 | (ans_2_3) | (ans_13 & ans_14);
				}
				{
					const unsigned int ans_11_12 = ans_11 & ans_12;
					possible &= ans_3 | (ans_10 & ans_11_12 & ((ans_8 & ans_9) | (ans_13 & ans_14)));
					possible &= ans_1 | (ans_11_12) | (ans_6 & ans_7);
					possible &= ans_6 | (ans_0 & ans_1) | (ans_11_12);
				}
				{
					const unsigned int ans_3_4 = ans_3 & ans_4;
					possible &= ans_9 | (ans_3_4) | (ans_14 & ans_15);
					possible &= ans_14 | (ans_8 & ans_9) | (ans_3_4);
				}
				{
					const unsigned int ans_10_11 = ans_10 & ans_11;
					possible &= ans_5 | (ans_15 & ans_0) | (ans_10_11);
					possible &= ans_0 | (ans_10_11) | (ans_5 & ans_6);
				}
				if (!possible) {
					continue;
				}
			}
#if COUNT_CHECKS
            check[8]++;
#endif

			possible |= (possible >> 16);

			//if(possible & 0x0f) //Does this make it faster?
			{
				if(possible & (1 << 0))
					corners.push_back(ImageRef(x + 0, y));
				if(possible & (1 << 1))
					corners.push_back(ImageRef(x + 1, y));
				if(possible & (1 << 2))
					corners.push_back(ImageRef(x + 2, y));
				if(possible & (1 << 3))
					corners.push_back(ImageRef(x + 3, y));
				if(possible & (1 << 4))
					corners.push_back(ImageRef(x + 4, y));
				if(possible & (1 << 5))
					corners.push_back(ImageRef(x + 5, y));
				if(possible & (1 << 6))
					corners.push_back(ImageRef(x + 6, y));
				if(possible & (1 << 7))
					corners.push_back(ImageRef(x + 7, y));
			}
			//if(possible & 0xf0) //Does this mak( ,  fast)r?
			{
				if(possible & (1 << 8))
					corners.push_back(ImageRef(x + 8, y));
				if(possible & (1 << 9))
					corners.push_back(ImageRef(x + 9, y));
				if(possible & (1 << 10))
					corners.push_back(ImageRef(x + 10, y));
				if(possible & (1 << 11))
					corners.push_back(ImageRef(x + 11, y));
				if(possible & (1 << 12))
					corners.push_back(ImageRef(x + 12, y));
				if(possible & (1 << 13))
					corners.push_back(ImageRef(x + 13, y));
				if(possible & (1 << 14))
					corners.push_back(ImageRef(x + 14, y));
				if(possible & (1 << 15))
					corners.push_back(ImageRef(x + 15, y));
			}

#if COUNT_CHECKS
            check[9]++;
#endif
		}

		for(int x = xend; x < I.size().x - 3; x++)
			if(CVD::is_corner_9<CVD::Less>(&I[y][x], I.row_stride(), barrier) || CVD::is_corner_9<CVD::Greater>(&I[y][x], I.row_stride(), barrier))
				corners.push_back(ImageRef(x, y));
	}
}

int main(int argc, char** argv) {
	string data_dir = "../data/";
	string out_dir = "../output/";

	string dir = fs::current_path().string();
	fs::create_directory(out_dir);

    for (auto& d : fs::directory_iterator(data_dir)) {
        string name = d.path().filename().string();
        string path = (d.path() / "frames").string();

        cout << path << endl;

#if COUNT_CHECKS
        FILE* outf = std::fopen((fs::path(out_dir) / (name + ".dat")).generic_u8string().c_str(), "wb");
#endif

        // For each threshold a vector of check with 8 exit counts
        for (int threshold = 0; threshold < 130; threshold += 10)
        {
			int image_count = 0;
			for (auto& f : fs::directory_iterator(path)) {

				// Load img from file
				CVD::Image<CVD::byte> img;
				CVD::img_load(img, f.path().string());
				uint64_t pixels = ((uint64_t)img.size().x - 3) * ((uint64_t)img.size().y - 3);

				uint64_t cvd_sse2_cycles = 0;
				uint64_t cvd_plain_cycles = 0;
				uint64_t unrolled_cycles = 0;
				uint64_t begin = 0;

				// Run SSE2 corner detection
				vector<CVD::ImageRef> ref;

				begin = __rdtsc();
				CVD::fast_corner_detect_9(img, ref, threshold);
				cvd_sse2_cycles += __rdtsc() - begin;
				sort(ref.begin(), ref.end());

				// CVD scalar corner detection
#if 0
				vector<CVD::ImageRef> plain;
				begin = __rdtsc();
				CVD::fast_corner_detect_plain_9(img, plain, threshold);
				cvd_plain_cycles += __rdtsc() - begin;
#endif

				// CVD ours corner
				vector<CVD::ImageRef> plain;
				begin = __rdtsc();
				our_faster_corner_detect_9(img, plain, threshold);
				cvd_plain_cycles += __rdtsc() - begin;
				sort(plain.begin(), plain.end());

				vector<CVD::ImageRef> plain_2;
				begin = __rdtsc();
				unrolled_faster(img, plain_2, threshold);
				unrolled_cycles += __rdtsc() - begin;
				sort(plain_2.begin(), plain_2.end());

				cout << ref.size() << " " << plain_2.size() << endl;
				// Sort corners
				if (!((ref == plain) && (ref == plain_2))) {
					cout << "Error" << endl;
					return 1;
				}

				//printf("\t\t%3d: %10d / %10d (%5.2f%%)\n", threshold, ref.size(), pixels, (double)ref.size() / pixels * 100);

				double cvd_sse2_perf = (double)cvd_sse2_cycles / (pixels * threshold);
				double cvd_plain_perf = (double)cvd_plain_cycles / (pixels * threshold);
				double cvd_plain_2_perf = (double)unrolled_cycles / (pixels * threshold);
				

				// printf("\t%s - sse2 %8.2f | plain %8.2f (cycles / pixel)\n", f.path().generic_u8string().c_str(), cvd_sse2_perf, cvd_plain_perf);

				image_count++;
			}

#if COUNT_CHECKS
            fprintf(outf, "%3d ", threshold);
            for (int i = 0; i < sizeof(check) / sizeof(check[0]); i++) {
                fprintf(outf, "%8.4f", ((double)check[i] / image_count) / ((double)check[0] / image_count));
            }
			for (int i = 0; i < sizeof(check) / sizeof(check[0]); i++) {
				check[i] = 0;
			}
			fprintf(outf, "\n");
#endif
		}
#if COUNT_CHECKS
        fclose(outf);
#endif
	}
    return 0;
}
