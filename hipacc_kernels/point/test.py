templ = """class Sub_Saturate_ONE_TWO_THREE : public Kernel<ONE_TYPE> {
private:
    Accessor<TWO_TYPE> &input1;
    Accessor<THREE_TYPE> &input2;

public:
    Sub_Saturate_ONE_TWO_THREE(IterationSpace<ONE_TYPE> &iter, Accessor<TWO_TYPE> &input1, Accessor<THREE_TYPE> &input2)
          : Kernel(iter), input1(input1), input2(input2){
        add_accessor(&input1);
        add_accessor(&input2);
    }

    void kernel() {
        int out = input1() - input2();

        if (out > 255) // max u8
            out = 255;
        else if (out < 0) // min u8
            out = 0;

        output() = out;
    }
};"""

"""        int out = abs((int) (input1()) + input2());

        if (out > 32767) // max s16
            out = 32767;
        else if (out < -32767) // min s16
            out = -32767;
"""

filename = """sub_saturate_ONE_TWO_THREE.hpp"""

outputs = []#["s16"]
outputs_type = ["short"]
inputs = ["u8", "s16"]
inputs_type = ["unsigned char", "short"]

for first in range(len(outputs)):
	for second in range(len(inputs)):
		for third in range(len(inputs)):
			with open(filename.replace("ONE", outputs[first]).replace("TWO", inputs[second]).replace("THREE", inputs[third]), "w") as f:
				f.write(templ.replace("ONE_TYPE", outputs_type[first]).replace("TWO_TYPE", inputs_type[second]).replace("THREE_TYPE", inputs_type[third]).replace("ONE", outputs[first]).replace("TWO", inputs[second]).replace("THREE", inputs[third]))


outputs = ["u8"]
outputs_type = ["unsigned char"]
inputs = ["u8"]#, "s16"]
inputs_type = ["unsigned char"]#, "short"]
for first in range(len(outputs)):
	for second in range(len(inputs)):
		for third in range(len(inputs)):
			with open(filename.replace("ONE", outputs[first]).replace("TWO", inputs[second]).replace("THREE", inputs[third]), "w") as f:
				f.write(templ.replace("ONE_TYPE", outputs_type[first]).replace("TWO_TYPE", inputs_type[second]).replace("THREE_TYPE", inputs_type[third]).replace("ONE", outputs[first]).replace("TWO", inputs[second]).replace("THREE", inputs[third]))
