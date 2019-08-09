class Copy_u8_u8 : public Kernel<unsigned char> {
private:
    Accessor<unsigned char> &input;

public:
    Copy_u8_u8(IterationSpace<unsigned char> &iter, Accessor<unsigned char> &input)
          : Kernel(iter), input(input){
        add_accessor(&input);
    }

    void kernel() {
        output() = input();
    }
};
