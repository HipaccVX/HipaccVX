class ThresholdBinary_u8_u8 : public Kernel<unsigned char> {
private:
    Accessor<unsigned char> &input;
    const int true_value;
    const int false_value;
    const int binary_threshold;

public:
    ThresholdBinary_u8_u8(IterationSpace<unsigned char> &iter, Accessor<unsigned char> &input,
    	int true_value, int false_value, int binary_threshold)
          : Kernel(iter), input(input), true_value(true_value), false_value(false_value), binary_threshold(binary_threshold){
        add_accessor(&input);
    }

    void kernel() {
    	int out = false_value;
    	if (input() > binary_threshold)
    		out = true_value;
        output() = out;
    }
};
