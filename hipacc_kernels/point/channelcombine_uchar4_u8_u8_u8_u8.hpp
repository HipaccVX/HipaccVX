class ChannelCombine_uchar4_u8_u8_u8_u8 : public Kernel<uchar4> {
private:
    Accessor<unsigned char> &input1;
    Accessor<unsigned char> &input2;
    Accessor<unsigned char> &input3;
    Accessor<unsigned char> &input4;

public:
    ChannelCombine_uchar4_u8_u8_u8_u8(IterationSpace<uchar4> &iter, Accessor<unsigned char> &input1, Accessor<unsigned char> &input2, Accessor<unsigned char> &input3, Accessor<unsigned char> &input4)
          : Kernel(iter), input1(input1), input2(input2), input3(input3), input4(input4){
        add_accessor(&input1);
        add_accessor(&input2);
        add_accessor(&input3);
        add_accessor(&input4);
    }

    void kernel() {
        output() = make_uchar4(input1(), input2(), input3(), input4());
    }
};
