
#include "hipacc.hpp"
#include "hipacc_helper.hpp"

using namespace hipacc;
using namespace hipacc::math;

class Kernel_0 : public Kernel<short>
{
private:
	Accessor<uchar>& Image_4;
	Domain& stencil_x;
	Mask<int>& stencil_x_mask;


public:
	Kernel_0(IterationSpace<short> &iter, Accessor<uchar>& Image_4, Domain& stencil_x, Mask<int>& stencil_x_mask)
		: Kernel(iter), Image_4(Image_4), stencil_x(stencil_x), stencil_x_mask(stencil_x_mask)
	{
		add_accessor(&Image_4);


	}

	void kernel() {
output()=reduce(stencil_x, Reduce::SUM, [&] () -> int {
return (stencil_x_mask(stencil_x)*Image_4(stencil_x));;

});

	}
};

class Kernel_2 : public Kernel<short>
{
private:
	Accessor<uchar>& Image_4;
	Domain& stencil_y;
	Mask<int>& stencil_y_mask;


public:
	Kernel_2(IterationSpace<short> &iter, Accessor<uchar>& Image_4, Domain& stencil_y, Mask<int>& stencil_y_mask)
		: Kernel(iter), Image_4(Image_4), stencil_y(stencil_y), stencil_y_mask(stencil_y_mask)
	{
		add_accessor(&Image_4);


	}

	void kernel() {
output()=reduce(stencil_y, Reduce::SUM, [&] () -> int {
return (stencil_y_mask(stencil_y)*Image_4(stencil_y));;

});

	}
};
class Convert_27 : public Kernel<uchar>
{
private:
	Accessor<short> &input;


public:
	Convert_27(IterationSpace<uchar> &iter, Accessor<short> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		uchar shift = 0;
		short value = input() >> shift;
		value = value < 0 ? 0 : value;
		value = value > 255 ? 255 : value;
		
		output() = (uchar) value;

	}
};
class Convert_28 : public Kernel<uchar>
{
private:
	Accessor<short> &input;


public:
	Convert_28(IterationSpace<uchar> &iter, Accessor<short> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		uchar shift = 0;
		short value = input() >> shift;
		value = value < 0 ? 0 : value;
		value = value > 255 ? 255 : value;
		
		output() = (uchar) value;

	}
};
class Simple_Point_30 : public Kernel<int>
{
private:
	Accessor<uchar> &input1;
	Accessor<uchar> &input2;


public:
	Simple_Point_30(IterationSpace<int> &iter, Accessor<uchar> &input1, Accessor<uchar> &input2)
		: Kernel(iter), input1(input1), input2(input2)
	{
		add_accessor(&input1);
		add_accessor(&input2);


	}

	void kernel() {
		output() = ( input1()) * input2();

	}
};

class Simple_Point_Scalar_31 : public Kernel<int>
{
private:
	Accessor<int> &input;


public:
	Simple_Point_Scalar_31(IterationSpace<int> &iter, Accessor<int> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		output() = input() * 1.000000;

	}
};

class Saturate_32 : public Kernel<uchar>
{
private:
	Accessor<int> &input;


public:
	Saturate_32(IterationSpace<uchar> &iter, Accessor<int> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		int out = input();
		if(1)
		{
			if (out > 255) // max u8
				out = 255;
			else if (out < 255) // min u8
				out = 0;
		}
		else if (0)
		{
			if (out > 32767) // max s16
				out = 32767;
			else if (out < -32768) // min s16
				out = -32768;
		}
		output() = out;

	}
};
class Simple_Point_34 : public Kernel<int>
{
private:
	Accessor<uchar> &input1;
	Accessor<uchar> &input2;


public:
	Simple_Point_34(IterationSpace<int> &iter, Accessor<uchar> &input1, Accessor<uchar> &input2)
		: Kernel(iter), input1(input1), input2(input2)
	{
		add_accessor(&input1);
		add_accessor(&input2);


	}

	void kernel() {
		output() = ( input1()) * input2();

	}
};

class Simple_Point_Scalar_35 : public Kernel<int>
{
private:
	Accessor<int> &input;


public:
	Simple_Point_Scalar_35(IterationSpace<int> &iter, Accessor<int> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		output() = input() * 1.000000;

	}
};

class Saturate_36 : public Kernel<uchar>
{
private:
	Accessor<int> &input;


public:
	Saturate_36(IterationSpace<uchar> &iter, Accessor<int> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		int out = input();
		if(1)
		{
			if (out > 255) // max u8
				out = 255;
			else if (out < 255) // min u8
				out = 0;
		}
		else if (0)
		{
			if (out > 32767) // max s16
				out = 32767;
			else if (out < -32768) // min s16
				out = -32768;
		}
		output() = out;

	}
};
class Simple_Point_38 : public Kernel<int>
{
private:
	Accessor<uchar> &input1;
	Accessor<uchar> &input2;


public:
	Simple_Point_38(IterationSpace<int> &iter, Accessor<uchar> &input1, Accessor<uchar> &input2)
		: Kernel(iter), input1(input1), input2(input2)
	{
		add_accessor(&input1);
		add_accessor(&input2);


	}

	void kernel() {
		output() = ( input1()) * input2();

	}
};

class Simple_Point_Scalar_39 : public Kernel<int>
{
private:
	Accessor<int> &input;


public:
	Simple_Point_Scalar_39(IterationSpace<int> &iter, Accessor<int> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		output() = input() * 1.000000;

	}
};

class Saturate_40 : public Kernel<uchar>
{
private:
	Accessor<int> &input;


public:
	Saturate_40(IterationSpace<uchar> &iter, Accessor<int> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		int out = input();
		if(1)
		{
			if (out > 255) // max u8
				out = 255;
			else if (out < 255) // min u8
				out = 0;
		}
		else if (0)
		{
			if (out > 32767) // max s16
				out = 32767;
			else if (out < -32768) // min s16
				out = -32768;
		}
		output() = out;

	}
};
class Linear_Mask_41 : public Kernel<uchar>
{
private:
	Accessor<uchar> &input;
	Domain &dom;
	Mask<int> &mask;


public:
	Linear_Mask_41(IterationSpace<uchar> &iter, Accessor<uchar> &input, Domain &dom, Mask<int> &mask)
		: Kernel(iter), input(input), dom(dom), mask(mask)
	{
		add_accessor(&input);


	}

	void kernel() {
		output() = reduce(dom, Reduce::SUM, [&] () -> short
			{
				return (mask(dom) * input(dom));
			}) * 0.062500;

	}
};
class Linear_Mask_44 : public Kernel<uchar>
{
private:
	Accessor<uchar> &input;
	Domain &dom;
	Mask<int> &mask;


public:
	Linear_Mask_44(IterationSpace<uchar> &iter, Accessor<uchar> &input, Domain &dom, Mask<int> &mask)
		: Kernel(iter), input(input), dom(dom), mask(mask)
	{
		add_accessor(&input);


	}

	void kernel() {
		output() = reduce(dom, Reduce::SUM, [&] () -> short
			{
				return (mask(dom) * input(dom));
			}) * 0.062500;

	}
};
class Linear_Mask_47 : public Kernel<uchar>
{
private:
	Accessor<uchar> &input;
	Domain &dom;
	Mask<int> &mask;


public:
	Linear_Mask_47(IterationSpace<uchar> &iter, Accessor<uchar> &input, Domain &dom, Mask<int> &mask)
		: Kernel(iter), input(input), dom(dom), mask(mask)
	{
		add_accessor(&input);


	}

	void kernel() {
		output() = reduce(dom, Reduce::SUM, [&] () -> short
			{
				return (mask(dom) * input(dom));
			}) * 0.062500;

	}
};
class Simple_Point_51 : public Kernel<int>
{
private:
	Accessor<uchar> &input1;
	Accessor<uchar> &input2;


public:
	Simple_Point_51(IterationSpace<int> &iter, Accessor<uchar> &input1, Accessor<uchar> &input2)
		: Kernel(iter), input1(input1), input2(input2)
	{
		add_accessor(&input1);
		add_accessor(&input2);


	}

	void kernel() {
		output() = ( input1()) - input2();

	}
};

class Saturate_52 : public Kernel<uchar>
{
private:
	Accessor<int> &input;


public:
	Saturate_52(IterationSpace<uchar> &iter, Accessor<int> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		int out = input();
		if(1)
		{
			if (out > 255) // max u8
				out = 255;
			else if (out < 255) // min u8
				out = 0;
		}
		else if (0)
		{
			if (out > 32767) // max s16
				out = 32767;
			else if (out < -32768) // min s16
				out = -32768;
		}
		output() = out;

	}
};
class Simple_Point_54 : public Kernel<int>
{
private:
	Accessor<uchar> &input1;
	Accessor<uchar> &input2;


public:
	Simple_Point_54(IterationSpace<int> &iter, Accessor<uchar> &input1, Accessor<uchar> &input2)
		: Kernel(iter), input1(input1), input2(input2)
	{
		add_accessor(&input1);
		add_accessor(&input2);


	}

	void kernel() {
		output() = ( input1()) + input2();

	}
};

class Saturate_55 : public Kernel<uchar>
{
private:
	Accessor<int> &input;


public:
	Saturate_55(IterationSpace<uchar> &iter, Accessor<int> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		int out = input();
		if(1)
		{
			if (out > 255) // max u8
				out = 255;
			else if (out < 255) // min u8
				out = 0;
		}
		else if (0)
		{
			if (out > 32767) // max s16
				out = 32767;
			else if (out < -32768) // min s16
				out = -32768;
		}
		output() = out;

	}
};
class Convert_56 : public Kernel<short>
{
private:
	Accessor<uchar> &input;


public:
	Convert_56(IterationSpace<short> &iter, Accessor<uchar> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		uchar shift = 0;
		uchar value = input() >> shift;
		value = value < 0 ? 0 : value;
		value = value > 255 ? 255 : value;
		
		output() = (short) value;

	}
};
class Convert_57 : public Kernel<short>
{
private:
	Accessor<uchar> &input;


public:
	Convert_57(IterationSpace<short> &iter, Accessor<uchar> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		uchar shift = 0;
		uchar value = input() >> shift;
		value = value < 0 ? 0 : value;
		value = value > 255 ? 255 : value;
		
		output() = (short) value;

	}
};
class Simple_Point_60 : public Kernel<int>
{
private:
	Accessor<short> &input1;
	Accessor<short> &input2;


public:
	Simple_Point_60(IterationSpace<int> &iter, Accessor<short> &input1, Accessor<short> &input2)
		: Kernel(iter), input1(input1), input2(input2)
	{
		add_accessor(&input1);
		add_accessor(&input2);


	}

	void kernel() {
		output() = ( input1()) * input2();

	}
};

class Simple_Point_62 : public Kernel<int>
{
private:
	Accessor<short> &input1;
	Accessor<short> &input2;


public:
	Simple_Point_62(IterationSpace<int> &iter, Accessor<short> &input1, Accessor<short> &input2)
		: Kernel(iter), input1(input1), input2(input2)
	{
		add_accessor(&input1);
		add_accessor(&input2);


	}

	void kernel() {
		output() = ( input1()) * input2();

	}
};

class Simple_Point_63 : public Kernel<int>
{
private:
	Accessor<int> &input1;
	Accessor<int> &input2;


public:
	Simple_Point_63(IterationSpace<int> &iter, Accessor<int> &input1, Accessor<int> &input2)
		: Kernel(iter), input1(input1), input2(input2)
	{
		add_accessor(&input1);
		add_accessor(&input2);


	}

	void kernel() {
		output() = ( input1()) + input2();

	}
};

class UnaryFunction_65 : public Kernel<int>
{
private:
	Accessor<int> &input;


public:
	UnaryFunction_65(IterationSpace<int> &iter, Accessor<int> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		output() = sqrt(input());

	}
};

class Saturate_66 : public Kernel<short>
{
private:
	Accessor<int> &input;


public:
	Saturate_66(IterationSpace<short> &iter, Accessor<int> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		int out = input();
		if(0)
		{
			if (out > 255) // max u8
				out = 255;
			else if (out < 255) // min u8
				out = 0;
		}
		else if (1)
		{
			if (out > 32767) // max s16
				out = 32767;
			else if (out < -32768) // min s16
				out = -32768;
		}
		output() = out;

	}
};
class Simple_Point_68 : public Kernel<int>
{
private:
	Accessor<uchar> &input1;
	Accessor<uchar> &input2;


public:
	Simple_Point_68(IterationSpace<int> &iter, Accessor<uchar> &input1, Accessor<uchar> &input2)
		: Kernel(iter), input1(input1), input2(input2)
	{
		add_accessor(&input1);
		add_accessor(&input2);


	}

	void kernel() {
		output() = ( input1()) + input2();

	}
};

class Saturate_69 : public Kernel<short>
{
private:
	Accessor<int> &input;


public:
	Saturate_69(IterationSpace<short> &iter, Accessor<int> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		int out = input();
		if(0)
		{
			if (out > 255) // max u8
				out = 255;
			else if (out < 255) // min u8
				out = 0;
		}
		else if (1)
		{
			if (out > 32767) // max s16
				out = 32767;
			else if (out < -32768) // min s16
				out = -32768;
		}
		output() = out;

	}
};
class Simple_Point_71 : public Kernel<int>
{
private:
	Accessor<short> &input1;
	Accessor<short> &input2;


public:
	Simple_Point_71(IterationSpace<int> &iter, Accessor<short> &input1, Accessor<short> &input2)
		: Kernel(iter), input1(input1), input2(input2)
	{
		add_accessor(&input1);
		add_accessor(&input2);


	}

	void kernel() {
		output() = ( input1()) + input2();

	}
};

class Saturate_72 : public Kernel<short>
{
private:
	Accessor<int> &input;


public:
	Saturate_72(IterationSpace<short> &iter, Accessor<int> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		int out = input();
		if(0)
		{
			if (out > 255) // max u8
				out = 255;
			else if (out < 255) // min u8
				out = 0;
		}
		else if (1)
		{
			if (out > 32767) // max s16
				out = 32767;
			else if (out < -32768) // min s16
				out = -32768;
		}
		output() = out;

	}
};
class Simple_Point_74 : public Kernel<int>
{
private:
	Accessor<short> &input1;
	Accessor<short> &input2;


public:
	Simple_Point_74(IterationSpace<int> &iter, Accessor<short> &input1, Accessor<short> &input2)
		: Kernel(iter), input1(input1), input2(input2)
	{
		add_accessor(&input1);
		add_accessor(&input2);


	}

	void kernel() {
		output() = ( input1()) - input2();

	}
};

class Saturate_75 : public Kernel<short>
{
private:
	Accessor<int> &input;


public:
	Saturate_75(IterationSpace<short> &iter, Accessor<int> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		int out = input();
		if(0)
		{
			if (out > 255) // max u8
				out = 255;
			else if (out < 255) // min u8
				out = 0;
		}
		else if (1)
		{
			if (out > 32767) // max s16
				out = 32767;
			else if (out < -32768) // min s16
				out = -32768;
		}
		output() = out;

	}
};
class Convert_76 : public Kernel<uchar>
{
private:
	Accessor<short> &input;


public:
	Convert_76(IterationSpace<uchar> &iter, Accessor<short> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		uchar shift = 0;
		short value = input() >> shift;
		value = value < 0 ? 0 : value;
		value = value > 255 ? 255 : value;
		
		output() = (uchar) value;

	}
};
class ConditionalAssignment_78 : public Kernel<uchar>
{
private:
	Accessor<uchar> &input;


public:
	ConditionalAssignment_78(IterationSpace<uchar> &iter, Accessor<uchar> &input)
		: Kernel(iter), input(input)
	{
		add_accessor(&input);


	}

	void kernel() {
		if (input() > 250)
			output() = 255;
		else
			output() = 0;

	}
};


int main(int argc, const char **argv)
{
	uchar *Image_4_input = load_data<uchar>(200, 300, 1, "akif-200x300_bw.png");
	Image<uchar> Image_4(200, 300, Image_4_input);
	Image<short> Image_5(200, 300);
	Image<short> Image_6(200, 300);
	Image<uchar> Image_9(200, 300);
	Image<uchar> Image_7(200, 300);
	Image<uchar> Image_8(200, 300);
	Image<uchar> Image_10(200, 300);
	Image<uchar> Image_11(200, 300);
	Image<uchar> Image_12(200, 300);
	Image<uchar> Image_13(200, 300);
	Image<uchar> Image_14(200, 300);
	Image<uchar> Image_15(200, 300);
	Image<uchar> Image_16(200, 300);
	Image<short> Image_17(200, 300);
	Image<short> Image_18(200, 300);
	Image<short> Image_19(200, 300);
	Image<short> Image_20(200, 300);
	Image<short> Image_21(200, 300);
	Image<uchar> Image_22(200, 300);
	Image<short> Image_23(200, 300);
	Image<uchar> Image_24(200, 300);
	Image<int> Image_80(200, 300);
	Image<int> Image_81(200, 300);
	Image<int> Image_82(200, 300);
	Image<int> Image_83(200, 300);
	Image<int> Image_84(200, 300);
	Image<int> Image_85(200, 300);
	Image<int> Image_86(200, 300);
	Image<int> Image_87(200, 300);
	Image<int> Image_88(200, 300);
	Image<int> Image_89(200, 300);
	Image<int> Image_90(200, 300);
	Image<int> Image_91(200, 300);
	Image<int> Image_92(200, 300);
	Image<int> Image_93(200, 300);
	Image<int> Image_94(200, 300);



	IterationSpace<short> iterspace_Image_5_0(
		Image_5);

	const int coef_mask_Image_4_0[3][3] = {
		{-1, 0, 1},
		{-2, 0, 2},
		{-1, 0, 1}};
	Mask<int> mask_Image_4_0(coef_mask_Image_4_0);

	Domain domain_Image_4_0(
		mask_Image_4_0);

	BoundaryCondition<uchar> boundary_Image_4_0(
		Image_4,
		domain_Image_4_0,
		Boundary::UNDEFINED);

	Accessor<uchar> accessor_Image_4_0(
		boundary_Image_4_0);

	Kernel_0 hipacc_kernel_call_0(
		iterspace_Image_5_0,
		accessor_Image_4_0,
		domain_Image_4_0,
		mask_Image_4_0);
	
	hipacc_kernel_call_0.execute();



	IterationSpace<short> iterspace_Image_6_2(
		Image_6);

	const int coef_mask_Image_4_2[3][3] = {
		{-1, -2, -1},
		{0, 0, 0},
		{1, 2, 1}};
	Mask<int> mask_Image_4_2(coef_mask_Image_4_2);

	Domain domain_Image_4_2(
		mask_Image_4_2);

	BoundaryCondition<uchar> boundary_Image_4_2(
		Image_4,
		domain_Image_4_2,
		Boundary::UNDEFINED);

	Accessor<uchar> accessor_Image_4_2(
		boundary_Image_4_2);

	Kernel_2 hipacc_kernel_call_2(
		iterspace_Image_6_2,
		accessor_Image_4_2,
		domain_Image_4_2,
		mask_Image_4_2);
	
	hipacc_kernel_call_2.execute();


	BoundaryCondition<short> bound_27(
		Image_5,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<short> acc_27(
		bound_27);

	IterationSpace<uchar> iter_27(
		Image_7);

	Convert_27 conv_27(
		iter_27,
		acc_27);
	
	conv_27.execute();


	BoundaryCondition<short> bound_28(
		Image_6,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<short> acc_28(
		bound_28);

	IterationSpace<uchar> iter_28(
		Image_8);

	Convert_28 conv_28(
		iter_28,
		acc_28);
	
	conv_28.execute();


	BoundaryCondition<uchar> bound_1_30(
		Image_7,
		200,
		300,
		Boundary::UNDEFINED);

	BoundaryCondition<uchar> bound_2_30(
		Image_7,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<uchar> acc_1_30(
		bound_1_30);

	Accessor<uchar> acc_2_30(
		bound_2_30);

	IterationSpace<int> iter_30(
		Image_80);

	Simple_Point_30 simple_point_30(
		iter_30,
		acc_1_30,
		acc_2_30);
	
	simple_point_30.execute();



	BoundaryCondition<int> bound_31(
		Image_80,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<int> acc_31(
		bound_31);

	IterationSpace<int> iter_31(
		Image_81);

	Simple_Point_Scalar_31 simple_point_scalar_31(
		iter_31,
		acc_31);
	
	simple_point_scalar_31.execute();



	BoundaryCondition<int> bound_32(
		Image_81,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<int> acc_32(
		bound_32);

	IterationSpace<uchar> iter_32(
		Image_9);

	Saturate_32 saturate_32(
		iter_32,
		acc_32);
	
	saturate_32.execute();


	BoundaryCondition<uchar> bound_1_34(
		Image_8,
		200,
		300,
		Boundary::UNDEFINED);

	BoundaryCondition<uchar> bound_2_34(
		Image_8,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<uchar> acc_1_34(
		bound_1_34);

	Accessor<uchar> acc_2_34(
		bound_2_34);

	IterationSpace<int> iter_34(
		Image_82);

	Simple_Point_34 simple_point_34(
		iter_34,
		acc_1_34,
		acc_2_34);
	
	simple_point_34.execute();



	BoundaryCondition<int> bound_35(
		Image_82,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<int> acc_35(
		bound_35);

	IterationSpace<int> iter_35(
		Image_83);

	Simple_Point_Scalar_35 simple_point_scalar_35(
		iter_35,
		acc_35);
	
	simple_point_scalar_35.execute();



	BoundaryCondition<int> bound_36(
		Image_83,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<int> acc_36(
		bound_36);

	IterationSpace<uchar> iter_36(
		Image_10);

	Saturate_36 saturate_36(
		iter_36,
		acc_36);
	
	saturate_36.execute();


	BoundaryCondition<uchar> bound_1_38(
		Image_7,
		200,
		300,
		Boundary::UNDEFINED);

	BoundaryCondition<uchar> bound_2_38(
		Image_8,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<uchar> acc_1_38(
		bound_1_38);

	Accessor<uchar> acc_2_38(
		bound_2_38);

	IterationSpace<int> iter_38(
		Image_84);

	Simple_Point_38 simple_point_38(
		iter_38,
		acc_1_38,
		acc_2_38);
	
	simple_point_38.execute();



	BoundaryCondition<int> bound_39(
		Image_84,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<int> acc_39(
		bound_39);

	IterationSpace<int> iter_39(
		Image_85);

	Simple_Point_Scalar_39 simple_point_scalar_39(
		iter_39,
		acc_39);
	
	simple_point_scalar_39.execute();



	BoundaryCondition<int> bound_40(
		Image_85,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<int> acc_40(
		bound_40);

	IterationSpace<uchar> iter_40(
		Image_11);

	Saturate_40 saturate_40(
		iter_40,
		acc_40);
	
	saturate_40.execute();


	const int coef_mask_41[3][3] = {
		{1, 2, 1},
		{2, 4, 2},
		{1, 2, 1}};
	Mask<int> mask_41(coef_mask_41);

	Domain dom_41(
		mask_41);

	BoundaryCondition<uchar> bound_41(
		Image_9,
		dom_41,
		Boundary::UNDEFINED);

	Accessor<uchar> acc_41(
		bound_41);

	IterationSpace<uchar> iter_41(
		Image_12);

	Linear_Mask_41 linear_41(
		iter_41,
		acc_41,
		dom_41,
		mask_41);
	
	linear_41.execute();


	const int coef_mask_44[3][3] = {
		{1, 2, 1},
		{2, 4, 2},
		{1, 2, 1}};
	Mask<int> mask_44(coef_mask_44);

	Domain dom_44(
		mask_44);

	BoundaryCondition<uchar> bound_44(
		Image_10,
		dom_44,
		Boundary::UNDEFINED);

	Accessor<uchar> acc_44(
		bound_44);

	IterationSpace<uchar> iter_44(
		Image_13);

	Linear_Mask_44 linear_44(
		iter_44,
		acc_44,
		dom_44,
		mask_44);
	
	linear_44.execute();


	const int coef_mask_47[3][3] = {
		{1, 2, 1},
		{2, 4, 2},
		{1, 2, 1}};
	Mask<int> mask_47(coef_mask_47);

	Domain dom_47(
		mask_47);

	BoundaryCondition<uchar> bound_47(
		Image_11,
		dom_47,
		Boundary::UNDEFINED);

	Accessor<uchar> acc_47(
		bound_47);

	IterationSpace<uchar> iter_47(
		Image_14);

	Linear_Mask_47 linear_47(
		iter_47,
		acc_47,
		dom_47,
		mask_47);
	
	linear_47.execute();


	BoundaryCondition<uchar> bound_1_51(
		Image_12,
		200,
		300,
		Boundary::UNDEFINED);

	BoundaryCondition<uchar> bound_2_51(
		Image_13,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<uchar> acc_1_51(
		bound_1_51);

	Accessor<uchar> acc_2_51(
		bound_2_51);

	IterationSpace<int> iter_51(
		Image_86);

	Simple_Point_51 simple_point_51(
		iter_51,
		acc_1_51,
		acc_2_51);
	
	simple_point_51.execute();



	BoundaryCondition<int> bound_52(
		Image_86,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<int> acc_52(
		bound_52);

	IterationSpace<uchar> iter_52(
		Image_15);

	Saturate_52 saturate_52(
		iter_52,
		acc_52);
	
	saturate_52.execute();


	BoundaryCondition<uchar> bound_1_54(
		Image_14,
		200,
		300,
		Boundary::UNDEFINED);

	BoundaryCondition<uchar> bound_2_54(
		Image_14,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<uchar> acc_1_54(
		bound_1_54);

	Accessor<uchar> acc_2_54(
		bound_2_54);

	IterationSpace<int> iter_54(
		Image_87);

	Simple_Point_54 simple_point_54(
		iter_54,
		acc_1_54,
		acc_2_54);
	
	simple_point_54.execute();



	BoundaryCondition<int> bound_55(
		Image_87,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<int> acc_55(
		bound_55);

	IterationSpace<uchar> iter_55(
		Image_16);

	Saturate_55 saturate_55(
		iter_55,
		acc_55);
	
	saturate_55.execute();


	BoundaryCondition<uchar> bound_56(
		Image_15,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<uchar> acc_56(
		bound_56);

	IterationSpace<short> iter_56(
		Image_17);

	Convert_56 conv_56(
		iter_56,
		acc_56);
	
	conv_56.execute();


	BoundaryCondition<uchar> bound_57(
		Image_16,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<uchar> acc_57(
		bound_57);

	IterationSpace<short> iter_57(
		Image_18);

	Convert_57 conv_57(
		iter_57,
		acc_57);
	
	conv_57.execute();


	BoundaryCondition<short> bound_1_60(
		Image_17,
		200,
		300,
		Boundary::UNDEFINED);

	BoundaryCondition<short> bound_2_60(
		Image_17,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<short> acc_1_60(
		bound_1_60);

	Accessor<short> acc_2_60(
		bound_2_60);

	IterationSpace<int> iter_60(
		Image_88);

	Simple_Point_60 simple_point_60(
		iter_60,
		acc_1_60,
		acc_2_60);
	
	simple_point_60.execute();



	BoundaryCondition<short> bound_1_62(
		Image_18,
		200,
		300,
		Boundary::UNDEFINED);

	BoundaryCondition<short> bound_2_62(
		Image_18,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<short> acc_1_62(
		bound_1_62);

	Accessor<short> acc_2_62(
		bound_2_62);

	IterationSpace<int> iter_62(
		Image_89);

	Simple_Point_62 simple_point_62(
		iter_62,
		acc_1_62,
		acc_2_62);
	
	simple_point_62.execute();



	BoundaryCondition<int> bound_1_63(
		Image_88,
		200,
		300,
		Boundary::UNDEFINED);

	BoundaryCondition<int> bound_2_63(
		Image_89,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<int> acc_1_63(
		bound_1_63);

	Accessor<int> acc_2_63(
		bound_2_63);

	IterationSpace<int> iter_63(
		Image_90);

	Simple_Point_63 simple_point_63(
		iter_63,
		acc_1_63,
		acc_2_63);
	
	simple_point_63.execute();



	BoundaryCondition<int> bound_65(
		Image_88,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<int> acc_65(
		bound_65);

	IterationSpace<int> iter_65(
		Image_91);

	UnaryFunction_65 unary_function_65(
		iter_65,
		acc_65);
	
	unary_function_65.execute();



	BoundaryCondition<int> bound_66(
		Image_91,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<int> acc_66(
		bound_66);

	IterationSpace<short> iter_66(
		Image_19);

	Saturate_66 saturate_66(
		iter_66,
		acc_66);
	
	saturate_66.execute();


	BoundaryCondition<uchar> bound_1_68(
		Image_12,
		200,
		300,
		Boundary::UNDEFINED);

	BoundaryCondition<uchar> bound_2_68(
		Image_13,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<uchar> acc_1_68(
		bound_1_68);

	Accessor<uchar> acc_2_68(
		bound_2_68);

	IterationSpace<int> iter_68(
		Image_92);

	Simple_Point_68 simple_point_68(
		iter_68,
		acc_1_68,
		acc_2_68);
	
	simple_point_68.execute();



	BoundaryCondition<int> bound_69(
		Image_92,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<int> acc_69(
		bound_69);

	IterationSpace<short> iter_69(
		Image_20);

	Saturate_69 saturate_69(
		iter_69,
		acc_69);
	
	saturate_69.execute();


	BoundaryCondition<short> bound_1_71(
		Image_20,
		200,
		300,
		Boundary::UNDEFINED);

	BoundaryCondition<short> bound_2_71(
		Image_19,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<short> acc_1_71(
		bound_1_71);

	Accessor<short> acc_2_71(
		bound_2_71);

	IterationSpace<int> iter_71(
		Image_93);

	Simple_Point_71 simple_point_71(
		iter_71,
		acc_1_71,
		acc_2_71);
	
	simple_point_71.execute();



	BoundaryCondition<int> bound_72(
		Image_93,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<int> acc_72(
		bound_72);

	IterationSpace<short> iter_72(
		Image_21);

	Saturate_72 saturate_72(
		iter_72,
		acc_72);
	
	saturate_72.execute();


	BoundaryCondition<short> bound_1_74(
		Image_20,
		200,
		300,
		Boundary::UNDEFINED);

	BoundaryCondition<short> bound_2_74(
		Image_19,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<short> acc_1_74(
		bound_1_74);

	Accessor<short> acc_2_74(
		bound_2_74);

	IterationSpace<int> iter_74(
		Image_94);

	Simple_Point_74 simple_point_74(
		iter_74,
		acc_1_74,
		acc_2_74);
	
	simple_point_74.execute();



	BoundaryCondition<int> bound_75(
		Image_94,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<int> acc_75(
		bound_75);

	IterationSpace<short> iter_75(
		Image_23);

	Saturate_75 saturate_75(
		iter_75,
		acc_75);
	
	saturate_75.execute();


	BoundaryCondition<short> bound_76(
		Image_21,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<short> acc_76(
		bound_76);

	IterationSpace<uchar> iter_76(
		Image_22);

	Convert_76 conv_76(
		iter_76,
		acc_76);
	
	conv_76.execute();


	BoundaryCondition<uchar> bound_78(
		Image_22,
		200,
		300,
		Boundary::UNDEFINED);

	Accessor<uchar> acc_78(
		bound_78);

	IterationSpace<uchar> iter_78(
		Image_24);

	ConditionalAssignment_78 conditional_assignment_78(
		iter_78,
		acc_78);
	
	conditional_assignment_78.execute();


	uchar *data_Image_24_79 = Image_24.data();
	save_data(200, 300, 1,  data_Image_24_79, "tomasi.png");




    return EXIT_SUCCESS;
}
