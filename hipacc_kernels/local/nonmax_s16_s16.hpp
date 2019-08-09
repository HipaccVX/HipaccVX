class NonMax_u8_u8 : public Kernel<short> {
private:
    Accessor<short> &input;
    Mask<int> &mask;
    Domain &dom;

public:
    NonMax_u8_u8(IterationSpace<short> &iter, Accessor<short> &input,
                    Domain &dom, Mask<int> &mask)
          : Kernel(iter), input(input), dom(dom), mask(mask){
        add_accessor(&input);
    }

    void kernel() {
        int suppressed = 0;
        short center = input();

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

        short value = center;
        if (suppressed == 1)
            value = -32767;
        output() = value;
    }
};
