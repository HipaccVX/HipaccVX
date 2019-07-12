class Convert_Wrap_u8_s16 : public Kernel<unsigned char> {
private:
    Accessor<short> &input;
    int shift;

public:
    Convert_Wrap_u8_s16(IterationSpace<unsigned char> &iter, Accessor<short> &input, int shift)
          : Kernel(iter), input(input), shift(shift){
        add_accessor(&input);
    }

    void kernel() {
        output() = ((unsigned char)(input() >> shift));
    }
};
