class Sub_Saturate_u8_u8_u8 : public Kernel<unsigned char> {
private:
    Accessor<unsigned char> &input1;
    Accessor<unsigned char> &input2;

public:
    Sub_Saturate_u8_u8_u8(IterationSpace<unsigned char> &iter, Accessor<unsigned char> &input1, Accessor<unsigned char> &input2)
          : Kernel(iter), input1(input1), input2(input2){
        add_accessor(&input1);
        add_accessor(&input2);
    }

    void kernel() {
        int out = input1() - input2();

        if (out > 255) // max u8
            out = 255;
        else if (out < 0) // min u8
            out = 0;

        output() = out;
    }
};