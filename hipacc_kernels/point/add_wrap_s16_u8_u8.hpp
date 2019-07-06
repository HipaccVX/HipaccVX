class Add_Wrap_s16_u8_u8 : public Kernel<short> {
private:
    Accessor<unsigned char> &input1;
    Accessor<unsigned char> &input2;

public:
    Add_Wrap_s16_u8_u8(IterationSpace<short> &iter, Accessor<unsigned char> &input1, Accessor<unsigned char> &input2)
          : Kernel(iter), input1(input1), input2(input2){
        add_accessor(&input1);
        add_accessor(&input2);
    }

    void kernel() {
        output() = input1() + input2();
    }
};