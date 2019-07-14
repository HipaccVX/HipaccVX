class HarrisMcVc_f_f_f : public Kernel<float> {
private:
    Accessor<float> &input1;
    Accessor<float> &input2;
    float k;
    float tc;

public:
    HarrisMcVc_f_f_f(IterationSpace<float> &iter, Accessor<float> &input1, Accessor<float> &input2, float k, float tc)
          : Kernel(iter), input1(input1), input2(input2), k(k), tc(tc){
        add_accessor(&input1);
        add_accessor(&input2);
    }

    void kernel() {
        float temp_2 = input2();
        float mc = input1() - k * temp_2 * temp_2;
        float vc = 0;
        if (mc > tc)
            vc = mc;
        output() = vc;
    }
};
