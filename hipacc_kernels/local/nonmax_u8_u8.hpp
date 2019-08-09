class NonMax_u8_u8 : public Kernel<unsigned char> {
private:
    Accessor<unsigned char> &input;
    Mask<int> &mask;
    Domain &dom;

public:
    NonMax_u8_u8(IterationSpace<unsigned char> &iter, Accessor<unsigned char> &input,
                    Domain &dom, Mask<int> &mask)
          : Kernel(iter), input(input), dom(dom), mask(mask){
        add_accessor(&input);
    }

    void kernel() {
        int suppressed = 0;
        unsigned char center = input();

        iterate(dom, [&, this] () -> void
        {
            if (dom.y() < 0 || (dom.y() == 0 && dom.x() < 0))
            {
                if (center < input(dom))
                    suppressed = 1;
            }
            else if (dom.y() != 0 || dom.x() != 0)
            {
                if (center <= input(dom))
                    suppressed = 1;
            }
        });

        unsigned char value = center;
        if (suppressed == 1)
            value = 0;
        output() = value;
    }
};
