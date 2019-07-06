class Gaussian_u8_u8 : public Kernel<unsigned char> {
private:
    Accessor<uchar> &input;
    Mask<int> &mask;
    Domain &dom;

public:
    Gaussian_u8_u8(IterationSpace<uchar> &iter, Accessor<uchar> &input,
                    Domain &dom, Mask<int> &mask)
          : Kernel(iter), input(input), dom(dom), mask(mask){
        add_accessor(&input);
    }

    void kernel() {
        output() = reduce(dom, Reduce::SUM, [&] () -> int {
            return mask(dom) * input(dom);
        }) / 16;
    }
};
