class Convert_s16_u8 : public Kernel<short> {
private:
    Accessor<unsigned char> &input;
    int shift;

public:
    Convert_s16_u8(IterationSpace<short> &iter, Accessor<unsigned char> &input, int shift)
          : Kernel(iter), input(input), shift(shift){
        add_accessor(&input);
    }

    void kernel() {
        output() = ((unsigned char)input()) << shift;
    }
};
