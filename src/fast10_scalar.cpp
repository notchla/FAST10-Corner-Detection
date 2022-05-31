#include "fast.h"

void fast10_scalar(uint8_t* data, uint32_t width, uint32_t height, uint32_t row_stride, std::vector<ImageRef>& corners, const int barrier)
{
	const int stride = 3 * row_stride;

	uint8_t c_barrier = (uint8_t)barrier;

	int xend = width  - 3;
	int yend = height - 3;

	for(int y = 3; y < yend; y++)
	{

		for(int x = 3; x < xend; x += 1)
		{
//			IACA_START

#if COUNT_CHECKS
            check[0]++;
#endif
#if COUNT_CHECK_POSITION
			lane_check[y * width + x] = 0;
#endif
			const uint8_t* p = data + y * row_stride + x;
			uint8_t lo, hi;
			{
				lo = *p - c_barrier < 0 ? 0 : *p - c_barrier;
				hi = *p + c_barrier > 255 ? 255 : *p + c_barrier;
			}

			unsigned int ans_b, ans_e;
			{
				uint8_t top = *(p - stride);
				uint8_t bottom = *(p + stride);

				ans_b = (lo > top) | ((top > hi) << 1); 
				ans_e = (lo > bottom) | ((bottom > hi) << 1); 

				if(!(ans_b | ans_e))
					continue;
			}

#if COUNT_CHECKS
            check[1]++;
#endif
#if COUNT_CHECK_POSITION
			lane_check[y * width + x]++;
#endif
			unsigned int ans_m, ans_p, possible;
			{
				uint8_t ul = *(p - 2 - 2 * row_stride);
				uint8_t lr = *(p + 2 + 2 * row_stride);

				ans_m = (lo > ul) | ((ul > hi) << 1); 
				ans_p = (lo > lr) | ((lr > hi) << 1); 

				possible = (ans_m & ans_b) | (ans_e & ans_p);
				if(!possible)
					continue;
			}
#if COUNT_CHECKS
            check[2]++;
#endif
#if COUNT_CHECK_POSITION
			lane_check[y * width + x]++;
#endif

			unsigned int ans_o, ans_n;
			{
				uint8_t ll = *(p - 2 + 2 * row_stride);
				uint8_t ur = *(p + 2 - 2 * row_stride);

				ans_o = (lo > ll) | ((ll > hi) << 1); 
				ans_n = (lo > ur) | ((ur > hi) << 1); 

				possible &= ans_o | (ans_b & ans_n);
				possible &= ans_n | (ans_e & ans_o);
				if(!possible)
					continue;
			}

#if COUNT_CHECKS
            check[3]++;
#endif
#if COUNT_CHECK_POSITION
			lane_check[y * width + x]++;
#endif
			unsigned int ans_h, ans_k;
			{
				uint8_t left = *(p - 3);
				uint8_t right = *(p + 3);
				
				ans_h = (lo > left) | ((left  > hi) << 1); 
				ans_k = (lo > right) | ((right > hi) << 1); 

				possible &= ans_h | (ans_n & ans_k & ans_p);
				possible &= ans_k | (ans_m & ans_h & ans_o);
				if(!possible)
					continue;
			}

#if COUNT_CHECKS
            check[4]++;
#endif
#if COUNT_CHECK_POSITION
			lane_check[y * width + x]++;
#endif
			unsigned int ans_a, ans_c;
			{
				uint8_t a = *(p - 1 - stride);
				uint8_t c = *(p + 1 - stride);

				ans_a = (lo > a) | ((a > hi) << 1); 
				ans_c = (lo > c) | ((c > hi) << 1); 

				possible &= ans_a | (ans_e & ans_p);
				possible &= ans_c | (ans_o & ans_e);
				if(!possible)
					continue;
			}

#if COUNT_CHECKS
            check[5]++;
#endif
#if COUNT_CHECK_POSITION
			lane_check[y * width + x]++;
#endif
			unsigned int ans_d, ans_f;
			{
				uint8_t d = *(p - 1 + stride);
				uint8_t f = *(p + 1 + stride);
				
				ans_d = (lo > d) | ((d > hi) << 1); 
				ans_f = (lo > f) | ((f > hi) << 1); 

				const unsigned int ans_abc = ans_a & ans_b & ans_c;
				possible &= ans_d | (ans_abc & ans_n);
				possible &= ans_f | (ans_m & ans_abc);
				if(!possible)
					continue;
			}

#if COUNT_CHECKS
            check[6]++;
#endif
#if COUNT_CHECK_POSITION
			lane_check[y * width + x]++;
#endif
			unsigned int ans_g, ans_i;
			{
				uint8_t g = *(p - 3 - row_stride);
				uint8_t ii = *(p - 3 + row_stride);

				ans_g = (lo > g) | ((g > hi) << 1); 
				ans_i = (lo > ii) | ((ii > hi) << 1); 

				possible &= ans_g | (ans_f & ans_p & ans_k);
				possible &= ans_i | (ans_c & ans_n & ans_k);
				if(!possible)
					continue;
			}

#if COUNT_CHECKS
            check[7]++;
#endif
#if COUNT_CHECK_POSITION
			lane_check[y * width + x]++;
#endif
			unsigned int ans_j, ans_l;
			{
				uint8_t jj = *(p + 3 - row_stride);
				uint8_t l = *(p + 3 + row_stride);
				
				ans_j = (lo > jj) | ((jj > hi) << 1); 
				ans_l = (lo > l) | ((l  > hi) << 1); 

				const unsigned int ans_ghi = ans_g & ans_h & ans_i;
				possible &= ans_j | (ans_d & ans_o & ans_ghi);
				possible &= ans_l | (ans_m & ans_a & ans_ghi);
				if(!possible)
					continue;
			}

#if COUNT_CHECKS
            check[8]++;
#endif
#if COUNT_CHECK_POSITION
			lane_check[y * width + x]++;
#endif
			possible |= (possible >> 1);
			
			if (possible) {
#if COUNT_CHECKS
                check[9]++;
#endif
#if COUNT_CHECK_POSITION
				lane_check[y * width + x]++;
#endif
				corners.push_back(ImageRef(x, y));
			}
				
		}

//		IACA_END

	}
}
