class ChannelExtract_3_u8_uchar4 : public Kernel<unsigned char> {
private:
    Accessor<uchar4> &input;

public:
    ChannelExtract_3_u8_uchar4(IterationSpace<unsigned char> &iter, Accessor<uchar4> &input)
          : Kernel(iter), input(input){
        add_accessor(&input);
    }

    void kernel() {
        output() = convert_uchar4(input()).w;
    }
};
