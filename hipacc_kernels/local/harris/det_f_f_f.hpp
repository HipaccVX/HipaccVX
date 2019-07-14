class HarrisDet_f_f_f : public Kernel<float> {
private:
    Accessor<float> &input1;
    Accessor<float> &input2;
    Mask<int> &mask;
    Domain &dom;

public:
    HarrisDet_f_f_f(IterationSpace<float> &iter, Accessor<float> &input1, Accessor<float> &input2,
                    Domain &dom, Mask<int> &mask)
          : Kernel(iter), input1(input1), input2(input2), dom(dom), mask(mask){
        add_accessor(&input1);
        add_accessor(&input2);
    }

    void kernel() {
        float gx2;
        float gy2;
        float gxy;
        iterate(dom, [&] () -> void {
            float temp_1 = input1(dom);
            float temp_2 = input2(dom);
            gx2 += temp_1 * temp_1;
            gy2 += temp_2 * temp_2;
            gxy = temp_1 + temp_2;
        });
        output() = gx2 * gy2 - (gxy * gxy);
    }
};
