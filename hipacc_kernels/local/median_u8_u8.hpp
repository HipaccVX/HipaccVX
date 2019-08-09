class Median3x3_u8_u8 : public Kernel<unsigned char> {
private:
    Accessor<uchar> &input;
    Mask<int> &mask;
    Domain &dom;

public:
    Median3x3_u8_u8(IterationSpace<uchar> &iter, Accessor<uchar> &input,
                    Domain &dom, Mask<int> &mask)
          : Kernel(iter), input(input), dom(dom), mask(mask){
        add_accessor(&input);
    }

    void kernel() {
        unsigned char values[9];
        unsigned char temp;
        int index = 0;
        iterate(dom, [&, this] () -> void
        {
            values[index++] = input(dom);
        });
        for(int i = 0; i < 9; i++)
        {
            for(int j = i + 1; j < 9; j++)
            {
                if(values[i] > values[j])
                {
                    temp = values[i];
                    values[i] = values[j];
                    values[j] = temp;
                }
            }
        }
        output() = values[5];
    }
};
