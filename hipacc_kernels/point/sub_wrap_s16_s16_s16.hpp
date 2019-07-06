class Sub_Wrap_s16_s16_s16 : public Kernel<short> {
private:
    Accessor<short> &input1;
    Accessor<short> &input2;

public:
    Sub_Wrap_s16_s16_s16(IterationSpace<short> &iter, Accessor<short> &input1, Accessor<short> &input2)
          : Kernel(iter), input1(input1), input2(input2){
        add_accessor(&input1);
        add_accessor(&input2);
    }

    void kernel() {
        output() = input1() - input2();
    }
};