class ThresholdRange_u8_u8 : public Kernel<unsigned char> {
private:
    Accessor<unsigned char> &input;
    const int true_value;
    const int false_value;
    const int lower_value;
    const int upper_value;

public:
    ThresholdRange_u8_u8(IterationSpace<unsigned char> &iter, Accessor<unsigned char> &input,
    	int true_value, int false_value, int lower_value, int upper_value)
          : Kernel(iter), input(input), true_value(true_value), false_value(false_value), lower_value(lower_value), upper_value(upper_value){
        add_accessor(&input);
    }

    void kernel() {
    	int out = true_value;
    	if (input() > upper_value)
    		out = false_value;
        else if (input() < lower_value)
            out = false_value;
        output() = out;
    }
};
