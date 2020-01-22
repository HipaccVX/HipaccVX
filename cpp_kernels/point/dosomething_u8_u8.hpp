void DoSomething_u8_u8(unsigned char *out_data, const unsigned int out_width, const unsigned int out_height,
                       unsigned char *in_data, const unsigned int in_width, const unsigned int in_height,
                       unsigned char to_add)
{
    const unsigned long long size = static_cast<unsigned long long>(out_width) * out_height;
    for (unsigned long long i = 0; i < size; i++)
    {
        out_data[i] = in_data[i] + to_add;
    }
}
