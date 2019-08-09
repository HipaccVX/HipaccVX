class Convolve_u8_u8 : public Kernel<unsigned char> {
private:
    Accessor<unsigned char> &input;
    Mask<short> &mask;
    Domain &dom;
    unsigned int scale;

public:
    Convolve_u8_u8(IterationSpace<unsigned char> &iter, Accessor<unsigned char> &input,
                    Domain &dom, Mask<short> &mask, unsigned int scale)
          : Kernel(iter), input(input), dom(dom), mask(mask), scale(scale){
        add_accessor(&input);
    }

    void kernel() {
        int sum = reduce(dom, Reduce::SUM, [&] () -> int {
            return mask(dom) * input(dom);
        });
        unsigned char out = 0;
        if (sum < 0)
            out = 0;
        else if (sum / scale > 255)
            out = 255;
        else
            out = sum / scale;
        output() = out;
    }
};
