class HarrisSobel_f_u8 : public Kernel<float> {
private:
    Accessor<unsigned char> &input;
    Mask<int> &mask;
    Domain &dom;
    int b;

public:
    HarrisSobel_f_u8(IterationSpace<float> &iter, Accessor<unsigned char> &input,
                    Domain &dom, Mask<int> &mask, int b)
          : Kernel(iter), input(input), dom(dom), mask(mask), b(b){
        add_accessor(&input);
    }

    void kernel() {
        output() = reduce(dom, Reduce::SUM, [&] () -> int {
            return input(dom) * mask(dom);
        }) * (1.f / (4 * 255 * b));
    }
};
