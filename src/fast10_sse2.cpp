#include "fast.h"

void fast10_scalar(const CVD::BasicImage<uint8_t>& I, std::vector<ImageRef>& corners, const int barrier)
{
	const int row_stride = I.row_stride();
	const int stride = 3 * row_stride;

	uint8_t c_barrier = (uint8_t)barrier;

	int xend = I.size().x - 3;
	int yend = I.size().y -3;

	for(int y = 3; y < yend; y++)
	{

		for(int x = 3; x < xend; x += 1)
		{
			const uint8_t* p = &I[y][x];
			//uint8_t lo, hi;

            __m128i lo, hi;

			{
				// lo = *p - c_barrier < 0 ? 0 : *p - c_barrier;
				// hi = *p + c_barrier > 255 ? 255 : *p + c_barrier;

                const __m128i here = _mm_load_si128((const __m128i*)(p));
                lo = _mm_subs_epu8(here, barriers);
                hi = _mm_adds_epu8(barriers, here);
			}

			unsigned int ans_b, ans_e;
			{
				// uint8_t top = *(p - stride);
				// uint8_t bottom = *(p + stride);

                __m128i top = _mm_load_si128((const __m128i*)(p - stride));
                __m128i bottom = _mm_load_si128((const __m128i*)(p + stride));

				ans_b = lo > top | ((top > hi) << 1); 
				ans_e = lo > bottom | ((bottom > hi) << 1); 

				if(!(ans_b | ans_e))
					continue;
			}

			unsigned int ans_m, ans_p, possible;
			{
				// uint8_t ul = *(p - 2 - 2 * row_stride);
				// uint8_t lr = *(p + 2 + 2 * row_stride);

                __m128i ul = _mm_load_si128((const __m128i*)(p - 2 - 2 * row_stride));
                __m128i lr = _mm_load_si128((const __m128i*)(p + 2 + 2 * row_stride));

				ans_m = lo > ul | ((ul > hi) << 1); 
				ans_p = lo > lr | ((lr > hi) << 1); 

				possible = (ans_m & ans_b) | (ans_e & ans_p);
				if(!possible)
					continue;
			}

			unsigned int ans_o, ans_n;
			{
				// uint8_t ll = *(p - 2 + 2 * row_stride);
				// uint8_t ur = *(p + 2 - 2 * row_stride);

                __m128i ll = _mm_load_si128((const __m128i*)(p - 2 + 2 * row_stride));
                __m128i ur = _mm_load_si128((const __m128i*)(p + 2 - 2 * row_stride));

				ans_o = lo > ll | ((ll > hi) << 1); 
				ans_n = lo > ur | ((ur > hi) << 1); 

				possible &= ans_o | (ans_b & ans_n);
				possible &= ans_n | (ans_e & ans_o);
				if(!possible)
					continue;
			}

			unsigned int ans_h, ans_k;
			{
				// uint8_t left = *(p - 3);
				// uint8_t right = *(p + 3);

                __m128i left = _mm_load_si128((const __m128i*)(p - 3));
                __m128i right = _mm_load_si128((const __m128i*)(p + 3));
				
				ans_h = lo > left  | ((left  > hi) << 1); 
				ans_k = lo > right | ((right > hi) << 1); 

				possible &= ans_h | (ans_n & ans_k & ans_p);
				possible &= ans_k | (ans_m & ans_h & ans_o);
				if(!possible)
					continue;
			}

			unsigned int ans_a, ans_c;
			{
				// uint8_t a = *(p - 1 - stride);
				// uint8_t c = *(p + 1 - stride);

                __m128i a = _mm_load_si128((const __m128i*)(p - 1 - stride));
                __m128i c = _mm_load_si128((const __m128i*)(p + 1 - stride));

				ans_a = lo > a | ((a > hi) << 1); 
				ans_c = lo > c | ((c > hi) << 1); 

				possible &= ans_a | (ans_e & ans_p);
				possible &= ans_c | (ans_o & ans_e);
				if(!possible)
					continue;
			}

			unsigned int ans_d, ans_f;
			{
				// uint8_t d = *(p - 1 + stride);
				// uint8_t f = *(p + 1 + stride);

                __m128i d = _mm_load_si128((const __m128i*)(p - 1 - stride));
                __m128i f = _mm_load_si128((const __m128i*)(p + 1 - stride));
				
				ans_d = lo > d | ((d > hi) << 1); 
				ans_f = lo > f | ((f > hi) << 1); 

				const unsigned int ans_abc = ans_a & ans_b & ans_c;
				possible &= ans_d | (ans_abc & ans_n);
				possible &= ans_f | (ans_m & ans_abc);
				if(!possible)
					continue;
			}

			unsigned int ans_g, ans_i;
			{
				// uint8_t g = *(p - 3 - row_stride);
				// uint8_t ii = *(p - 3 + row_stride);

                __m128i g = _mm_load_si128((const __m128i*)(p - 3 - row_stride));
                __m128i ii = _mm_load_si128((const __m128i*)(p - 3 + row_stride));


				ans_g = lo > g  | ((g > hi) << 1); 
				ans_i = lo > ii | ((ii > hi) << 1); 

				possible &= ans_g | (ans_f & ans_p & ans_k);
				possible &= ans_i | (ans_c & ans_n & ans_k);
				if(!possible)
					continue;
			}

			unsigned int ans_j, ans_l;
			{
				// uint8_t jj = *(p + 3 - row_stride);
				// uint8_t l = *(p + 3 + row_stride);

                __m128i g = _mm_load_si128((const __m128i*)(p + 3 - row_stride));
                __m128i ii = _mm_load_si128((const __m128i*)(p + 3 + row_stride));
				
				ans_j = lo > jj | ((jj > hi) << 1); 
				ans_l = lo > l  | ((l  > hi) << 1); 

				const unsigned int ans_ghi = ans_g & ans_h & ans_i;
				possible &= ans_j | (ans_d & ans_o & ans_ghi);
				possible &= ans_l | (ans_m & ans_a & ans_ghi);
				if(!possible)
					continue;
			}

			possible |= (possible >> 1);
			
			if(possible)
				corners.push_back(ImageRef(x , y));
				
		}

	}
}
