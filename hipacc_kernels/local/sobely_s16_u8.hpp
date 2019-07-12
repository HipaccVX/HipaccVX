class SobelY_s16_u8 : public Kernel<short> {
private:
    Accessor<unsigned char> &input;
    Mask<int> &mask;
    Domain &dom;

public:
    SobelY_s16_u8(IterationSpace<short> &iter, Accessor<unsigned char> &input,
                    Domain &dom, Mask<int> &mask)
          : Kernel(iter), input(input), dom(dom), mask(mask){
        add_accessor(&input);
    }

    void kernel() {
        output() = reduce(dom, Reduce::SUM, [&] () -> int {
            return mask(dom) * input(dom);
        });
    }
};
