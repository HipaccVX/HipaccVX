class Magnitude_s16_s16_s16 : public Kernel<short> {
private:
    Accessor<short> &input1;
    Accessor<short> &input2;

public:
    Magnitude_s16_s16_s16(IterationSpace<short> &iter, Accessor<short> &input1, Accessor<short> &input2)
          : Kernel(iter), input1(input1), input2(input2){
        add_accessor(&input1);
        add_accessor(&input2);
    }

    void kernel() {
        short in_1 = input1();
        short in_2 = input2();
        unsigned int in1_sq = int(in_1) * int(in_1);
        unsigned int in2_sq = int(in_2) * int(in_2);
        unsigned short z = sqrt(double(in1_sq + in2_sq)) + 0.5;

        output() = z > 32767 ? 32767 : z;
    }
};
