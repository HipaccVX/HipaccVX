void Fast9_u8(Keypoint *out_corners, const int max_corners, int32_t *out_num_corners,
              unsigned char *in_data, const unsigned int in_width, const unsigned int in_height,
              float strength_thresh, bool nonmax_supression);

const int PERMUTATIONS = 16;
const int APERTURE = 3;

static uint8_t fast9_indexes[PERMUTATIONS][9] = {
    { 0, 1, 2, 3, 4, 5, 6, 7, 8},
    {15, 0, 1, 2, 3, 4, 5, 6, 7},
    {14,15, 0, 1, 2, 3, 4, 5, 6},
    {13,14,15, 0, 1, 2, 3, 4, 5},
    {12,13,14,15, 0, 1, 2, 3, 4},
    {11,12,13,14,15, 0, 1, 2, 3},
    {10,11,12,13,14,15, 0, 1, 2},
    { 9,10,11,12,13,14,15, 0, 1},
    { 8, 9,10,11,12,13,14,15, 0},
    { 7, 8, 9,10,11,12,13,14,15},
    { 6, 7, 8, 9,10,11,12,13,14},
    { 5, 6, 7, 8, 9,10,11,12,13},
    { 4, 5, 6, 7, 8, 9,10,11,12},
    { 3, 4, 5, 6, 7, 8, 9,10,11},
    { 2, 3, 4, 5, 6, 7, 8, 9,10},
    { 1, 2, 3, 4, 5, 6, 7, 8, 9},
};

/* offsets from "p" */
static int32_t fast9_offsets[16][2] = {
    {  0, -3},
    {  1, -3},
    {  2, -2},
    {  3, -1},
    {  3,  0},
    {  3,  1},
    {  2,  2},
    {  1,  3},
    {  0,  3},
    { -1,  3},
    { -2,  2},
    { -3,  1},
    { -3,  0},
    { -3, -1},
    { -2, -2},
    { -1, -3},
};


static bool fast9_IsFastCorner(const uint8_t* buf, uint8_t p, uint8_t tolerance)
{
    uint32_t i, a;
    for (a = 0; a < PERMUTATIONS; a++)
    {
        bool isacorner = true;
        for (i = 0; i < 2; i++)
        {
            uint8_t j = fast9_indexes[a][i];
            uint8_t v = buf[j];
            if (v <= (p + tolerance))
            {
                isacorner = false;
            }
        }
        if (isacorner == true)
            return isacorner;
        isacorner = true;
        for (i = 0; i < 2; i++)
        {
            uint8_t j = fast9_indexes[a][i];
            uint8_t v = buf[j];
            if (v >= (p - tolerance))
            {
                isacorner = false;
            }
        }
        if (isacorner == true)
            return isacorner;
    }
    return false;
}


static uint8_t fast9_GetFastCornerStrength(int32_t x, int32_t y,
                                           unsigned char *in_data, const unsigned int in_width, const unsigned int in_height,
                                           uint8_t tolerance)
{
    if (x < APERTURE || y < APERTURE || x >= (int32_t)(in_width) - APERTURE || y >= (int32_t)(in_height) - APERTURE)
        return 0;
    {
    uint8_t p = in_data[y * in_width + x];
    uint8_t buf[16];
    int32_t j;
    uint8_t a, b = 255;

    for (j = 0; j < 16; j++)
    {
        buf[j] = in_data[(y+fast9_offsets[j][1]) * in_width + (x+fast9_offsets[j][0])];
    }

    if (!fast9_IsFastCorner(buf, p, tolerance))
        return 0;

    a = tolerance;
    while (b - a > 1)
    {
        uint8_t c = (a + b)/2;
        if (fast9_IsFastCorner(buf, p, c))
            a = c;
        else
            b = c;
    }
    return a;
    }
}



void Fast9_u8(Keypoint *out_corners, const int max_corners, int32_t *out_num_corners,
              unsigned char *in_data, const unsigned int in_width, const unsigned int in_height,
              float strength_thresh, bool nonmax_supression)
{
    int num_corners = 0;
    uint8_t tolerance = strength_thresh;
    // Bordermode undefined only
	for (long long y = APERTURE; y < in_height - APERTURE; y++)
	{
		for (long long x = APERTURE; x < in_width - APERTURE; x++)
		{
			vx_uint8 strength = fast9_GetFastCornerStrength(x, y, in_data, in_width, in_height, tolerance);
			if (strength > 0)
			{
				if (nonmax_supression)
				{
					if (strength >= fast9_GetFastCornerStrength(x-1, y-1, in_data, in_width, in_height, tolerance) &&
						strength >= fast9_GetFastCornerStrength(x,   y-1, in_data, in_width, in_height, tolerance) &&
						strength >= fast9_GetFastCornerStrength(x+1, y-1, in_data, in_width, in_height, tolerance) &&
						strength >= fast9_GetFastCornerStrength(x-1, y,   in_data, in_width, in_height, tolerance) &&
						strength >  fast9_GetFastCornerStrength(x+1, y,   in_data, in_width, in_height, tolerance) &&
						strength >  fast9_GetFastCornerStrength(x-1, y+1, in_data, in_width, in_height, tolerance) &&
						strength >  fast9_GetFastCornerStrength(x,   y+1, in_data, in_width, in_height, tolerance) &&
						strength >  fast9_GetFastCornerStrength(x+1, y+1, in_data, in_width, in_height, tolerance))
						;
					else
						continue;
				}
				if (num_corners < max_corners)
				{
					out_corners[num_corners].x = x;
					out_corners[num_corners].y = y;
					out_corners[num_corners].strength = strength;
					out_corners[num_corners].scale = 0.0f;
					out_corners[num_corners].orientation = 0.0f;
					out_corners[num_corners].tracking_status = 1;
					out_corners[num_corners].error = 0.0f;
				}
				num_corners++;
			}
		}
	}
	if (out_num_corners != nullptr)
		*out_num_corners = num_corners;
}
