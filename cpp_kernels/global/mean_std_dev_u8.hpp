void Mean_Std_Dev_u8(float *out_mean, float *out_stddev,
                     unsigned char *in_data, const unsigned int in_width, const unsigned int in_height)
{
	float fmean = 0.0f, fstddev = 0.0f;
	double sum = 0.0, sum_diff_sqrs = 0.0;

	for (long long y = 0; y < in_height; y++)
	{
		for (long long x = 0; x < in_width; x++)
		{
			sum += in_data[y * in_width + x];
		}
	}
	fmean = (float)(sum / (in_width * in_height));
	for (long long y = 0; y < in_height; y++)
	{
		for (long long x = 0; x < in_width; x++)
		{
			double value = in_data[y * in_width + x] - fmean;
			sum_diff_sqrs += value * value;
		}
	}
	fstddev = (float)sqrt(sum_diff_sqrs / (in_width * in_height));
	*out_mean = fmean;
	if (out_stddev != nullptr)
		*out_stddev = fstddev;
}
