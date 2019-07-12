class Convert_Saturate_u8_s16 : public Kernel<short> {
private:
    Accessor<unsigned char> &input;
    int shift;

public:
    Convert_Saturate_u8_s16(IterationSpace<short> &iter, Accessor<unsigned char> &input, int shift)
          : Kernel(iter), input(input), shift(shift){
        add_accessor(&input);
    }

    void kernel() {
        short value = input() >> shift;
        value = value < 0 ? 0 : value;
        value = value > 255 ? 255 : value;
        output() = (unsigned char)value;
    }
};
