class Convolve_s16_u8 : public Kernel<short> {
private:
    Accessor<unsigned char> &input;
    Mask<short> &mask;
    Domain &dom;
    unsigned int scale;

public:
    Convolve_s16_u8(IterationSpace<short> &iter, Accessor<unsigned char> &input,
                    Domain &dom, Mask<short> &mask, unsigned int scale)
          : Kernel(iter), input(input), dom(dom), mask(mask), scale(scale){
        add_accessor(&input);
    }

    void kernel() {
        int sum = reduce(dom, Reduce::SUM, [&] () -> int {
            return mask(dom) * input(dom);
        });
        output() = sum / scale;
    }
};
