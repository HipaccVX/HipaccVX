void Integral_u32_u8(uint32_t *out_data, const unsigned int out_width, const unsigned int out_height,
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
			if (y == 0)
			{
				out_data[y * out_width] = in_data[y * out_width];
				for (x = 1; x < out_width; x++)
				{
					out_data[y * out_width + x] = out_data[y * out_width + (x-1)] + in_data[y * out_width];
				}
			}
			else
			{
				out_data[y * out_width] = out_data[(y-1) * out_width] + in_data[y * out_width];
				for (x = 1; x < out_width; x++)
				{
					out_data[y * out_width] = in_data[y * out_width] + out_data[y * out_width + (x-1)] +
											  out_data[(y-1) * out_width] - out_data[(y-1) * out_width + (x-1)];
				}
			}
		}
	}
}
