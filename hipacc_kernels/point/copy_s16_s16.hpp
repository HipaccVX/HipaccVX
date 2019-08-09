class Copy_s16_16 : public Kernel<short> {
private:
    Accessor<short> &input;

public:
    Copy_s16_16(IterationSpace<short> &iter, Accessor<short> &input)
          : Kernel(iter), input(input){
        add_accessor(&input);
    }

    void kernel() {
        output() = input();
    }
};
