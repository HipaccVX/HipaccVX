class Mul_Saturate_s16_u8_s16 : public Kernel<short> {
private:
    Accessor<unsigned char> &input1;
    Accessor<short> &input2;
    float scale;

public:
    Mul_Saturate_s16_u8_s16(IterationSpace<short> &iter,Accessor<unsigned char> &input1, Accessor<short> &input2, float scale)
          : Kernel(iter), input1(input1), input2(input2), scale(scale) {
        add_accessor(&input1);
        add_accessor(&input2);
    }

    void kernel() {
        int out = input1() * input2() * scale;

        if (out > 32767) // max s16
            out = 32767;
        else if (out < -32767) // min s16
            out = -32767;

        output() = out;
    }
};