class Copy_f_f : public Kernel<float> {
private:
    Accessor<float> &input;

public:
    Copy_f_f(IterationSpace<float> &iter, Accessor<float> &input)
          : Kernel(iter), input(input){
        add_accessor(&input);
    }

    void kernel() {
        output() = input();
    }
};
