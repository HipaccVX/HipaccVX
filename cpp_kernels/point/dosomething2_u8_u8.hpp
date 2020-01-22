void DoSomething2_u8_u8(unsigned char *out_data, const unsigned int out_width, const unsigned int out_height,
                        unsigned char *in_data, const unsigned int in_width, const unsigned int in_height,
                        int* matrix_data, const unsigned int matrix_width, const unsigned int matrix_height)
{
    const unsigned long long size = static_cast<unsigned long long>(out_width) * out_height;
	for (long long y = 0; y < out_height; y++)
	{
		for (long long x = 0; x < out_width; x++)
		{
			int sum = 0;
			for (long long m_y = 0; m_y < matrix_height; m_y++)
			{
				for (long long m_x = 0; m_x < matrix_width; m_x++)
				{
					const long long rel_y = m_y - (matrix_height / 2);
					const long long rel_x = m_x - (matrix_width / 2);
					const long long curr_y = y + rel_y;
					const long long curr_x = x + rel_x;

					if (curr_y < 0 || curr_y >= out_height)
						continue;
					if (curr_x < 0 || curr_x >= out_width)
						continue;
					sum += in_data[curr_y * out_width + curr_x] * matrix_data[m_y * matrix_width + m_x];
				}
			}
			out_data[y * out_width + x] = sum/16;
		}
	}
}
