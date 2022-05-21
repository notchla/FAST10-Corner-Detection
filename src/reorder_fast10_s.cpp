#include "fast.h"

void reorder_fast10_s(const CVD::BasicImage<uint8_t>& I, std::vector<ImageRef>& corners, const int barrier)
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

			#if COUNT_CHECKS
            	check[0]++;
			#endif

			const uint8_t* p = &I[y][x];
			uint8_t lo, hi;
			{
				lo = *p - c_barrier < 0 ? 0 : *p - c_barrier;
				hi = *p + c_barrier > 255 ? 255 : *p + c_barrier;
			}

			unsigned int ans_b, ans_e;
			{
				uint8_t r_12 = *(p - 3);
				uint8_t r_4 = *(p + 3);

				ans_b = lo > r_12 | ((r_12 > hi) << 1); 
				ans_e = lo > r_4 | ((r_4 > hi) << 1); 

				if(!(ans_b | ans_e))
					continue;
			}

			#if COUNT_CHECKS
            	check[1]++;
			#endif

			unsigned int ans_m, ans_p, possible;
			{
				uint8_t r_10 = *(p - 2 + 2 * row_stride);
				uint8_t r_2 = *(p + 2 - 2 * row_stride);

				ans_m = lo > r_10 | ((r_10 > hi) << 1); 
				ans_p = lo > r_2 | ((r_2 > hi) << 1); 

				possible = (ans_m & ans_b) | (ans_e & ans_p);
				if(!possible)
					continue;
			}

			#if COUNT_CHECKS
            	check[2]++;
			#endif

			unsigned int ans_o, ans_n;
			{
				uint8_t r_6 = *(p + 2 + 2 * row_stride);
				uint8_t r_14 = *(p - 2 - 2 * row_stride);

				ans_o = lo > r_6 | ((r_6 > hi) << 1); 
				ans_n = lo > r_14 | ((r_14 > hi) << 1); 

				possible &= ans_o | (ans_b & ans_n);
				possible &= ans_n | (ans_e & ans_o);
				if(!possible)
					continue;
			}

			#if COUNT_CHECKS
            	check[3]++;
			#endif

			unsigned int ans_h, ans_k;
			{
				uint8_t r_8 = *(p + stride);
				uint8_t r_0 = *(p - stride);
				
				ans_h = lo > r_8  | ((r_8  > hi) << 1); 
				ans_k = lo > r_0 | ((r_0 > hi) << 1); 

				possible &= ans_h | (ans_n & ans_k & ans_p);
				possible &= ans_k | (ans_m & ans_h & ans_o);
				if(!possible)
					continue;
			}

			#if COUNT_CHECKS
            	check[4]++;
			#endif

			unsigned int ans_a, ans_c;
			{
				uint8_t r_11 = *(p - 3 + row_stride);
				uint8_t r_13 = *(p - 3 - row_stride);

				ans_a = lo > r_11 | ((r_11 > hi) << 1); 
				ans_c = lo > r_13 | ((r_13 > hi) << 1); 

				possible &= ans_a | (ans_e & ans_p);
				possible &= ans_c | (ans_o & ans_e);
				if(!possible)
					continue;
			}
			
			#if COUNT_CHECKS
            	check[5]++;
			#endif

			unsigned int ans_d, ans_f;
			{
				uint8_t r_5 = *(p + 3 + row_stride);
				uint8_t r_3 = *(p + 3 - row_stride);
				
				ans_d = lo > r_5 | ((r_5 > hi) << 1); 
				ans_f = lo > r_3 | ((r_3 > hi) << 1); 

				const unsigned int ans_abc = ans_a & ans_b & ans_c;
				possible &= ans_d | (ans_abc & ans_n);
				possible &= ans_f | (ans_m & ans_abc);
				if(!possible)
					continue;
			}
			
			#if COUNT_CHECKS
            	check[6]++;
			#endif

			unsigned int ans_g, ans_i;
			{
				uint8_t r_9 = *(p - 1 + stride);
				uint8_t r_7 = *(p + 1 + stride);

				ans_g = lo > r_9  | ((r_9 > hi) << 1); 
				ans_i = lo > r_7 | ((r_7 > hi) << 1); 

				possible &= ans_g | (ans_f & ans_p & ans_k);
				possible &= ans_i | (ans_c & ans_n & ans_k);
				if(!possible)
					continue;
			}

			#if COUNT_CHECKS
            	check[7]++;
			#endif

			unsigned int ans_j, ans_l;
			{
				uint8_t r_15 = *(p - 1 - stride);
				uint8_t r_1 = *(p + 1 - stride);
				
				ans_j = lo > r_15 | ((r_15 > hi) << 1); 
				ans_l = lo > r_1  | ((r_1  > hi) << 1); 

				const unsigned int ans_ghi = ans_g & ans_h & ans_i;
				possible &= ans_j | (ans_d & ans_o & ans_ghi);
				possible &= ans_l | (ans_m & ans_a & ans_ghi);
				if(!possible)
					continue;
			}
			
			#if COUNT_CHECKS
            	check[8]++;
			#endif

			possible |= (possible >> 1);
			
			if(possible)
				corners.push_back(ImageRef(x , y));
				
		}

	}
}
