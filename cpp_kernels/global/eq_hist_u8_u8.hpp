void Eq_Hist_u8_u8(unsigned char *out_data, const unsigned int out_width, const unsigned int out_height,
              unsigned char *in_data, const unsigned int in_width, const unsigned int in_height)
{
	const int NUM_BINS = 256;

	uint32_t hist[NUM_BINS] = {0};
	uint32_t cdf[NUM_BINS] = {0};
	uint32_t sum = 0, div;
	uint8_t minv = 0xFF;


	for (long long y = 0; y < out_height; y++)
	{
		for (long long x = 0; x < out_width; x++)
		{
			uint8_t pixel = in_data[y * out_width + x];
			hist[pixel]++;
			if (minv > pixel)
				minv = pixel;
		}
	}
	for (int x = 0; x < NUM_BINS; x++)
	{
		sum += hist[x];
		cdf[x] = sum;
	}
	div = (out_width * out_height) - cdf[minv];
	if( div > 0 )
	{
		/* recompute the histogram to be a LUT for replacing pixel values */
		for (int x = 0; x < NUM_BINS; x++)
		{
			uint32_t cdfx = cdf[x] - cdf[minv];
			float p = (float)cdfx/(float)div;
			hist[x] = (uint8_t)(p * 255.0f + 0.5f);
		}
	}
	else
	{
		for (int x = 0; x < NUM_BINS; x++)
		{
			hist[x] = x;
		}
	}

	/* map the src pixel values to the equalized pixel values */
	for (long long y = 0; y < out_height; y++)
	{
		for (long long x = 0; x < out_width; x++)
		{
			out_data[y * out_width + x] = hist[in_data[y * out_width + x]];
		}
	}
}
