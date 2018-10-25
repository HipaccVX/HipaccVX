#include "hipaVX_internal.hpp"
#include "hipaVX_extension.hpp"
#include <fstream>
#include <string>

namespace HipaVX
{

int Object::next_id = 0;
Object::Object()
	:my_id(next_id++)
{
}

Scalar::Scalar(vx_type_e t, const void *ptr)
	:data_type(t)
{
	type = VX_TYPE_SCALAR;
	switch(data_type)
	{
	case VX_TYPE_INT32:
		i32 = *((vx_int32*) ptr);
		break;
	case VX_TYPE_FLOAT32:
		f32 = *((vx_float32*) ptr);
		break;
	default:
		throw std::runtime_error("Unsupported vx_type_e in Scalar constructor");
	}
}

Image::Image(vx_uint32 width, vx_uint32 height, vx_df_image color)
	:w(width), h(height), col(color)
{
	type = VX_TYPE_IMAGE;
}

Array::Array(vx_enum item_type, vx_size cap, vx_size rows)
	:Image(rows, cap, VX_DF_IMAGE_S32), type(item_type), capacity(cap)
{
	type = VX_TYPE_ARRAY;
}

FileinputImage::FileinputImage(vx_uint32 width, vx_uint32 height, vx_df_image color, std::string filename)
	:Image(width, height, color), file(filename)
{
}


void Graph::build()
{
	for (auto& node: graph)
	{
		node->build();
	}

	built = true;
}

WriteImageNode::WriteImageNode()
{
	node_name = "Image Writer";
}
std::vector<Object *> WriteImageNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	return used_objects;
}
std::vector<Object *> WriteImageNode::get_outputs()
{
	std::vector<Object*> used_objects;
	return used_objects;
}
std::string WriteImageNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string WriteImageNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

ConvertDepthNode::ConvertDepthNode()
{
	node_name = "Depth Converter";
}
std::vector<Object *> ConvertDepthNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	used_objects.emplace_back(shift);
	return used_objects;
}
std::vector<Object *> ConvertDepthNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::string ConvertDepthNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string ConvertDepthNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

NotNode::NotNode()
{
	node_name = "Not";
}
std::vector<Object *> NotNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	return used_objects;
}
std::vector<Object *> NotNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::string NotNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string NotNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

Dilate::Dilate()
{
	node_name = "Dilate";
}
std::vector<Object *> Dilate::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	return used_objects;
}
std::vector<Object *> Dilate::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::string Dilate::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string Dilate::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

Erode::Erode()
{
	node_name = "Erode";
}
std::vector<Object *> Erode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	return used_objects;
}
std::vector<Object *> Erode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::string Erode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string Erode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}


VXThresholdNode::VXThresholdNode()
{
	node_name = "VX Threshold";
}
std::vector<Object *> VXThresholdNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	used_objects.emplace_back(threshold);
	return used_objects;
}
std::vector<Object *> VXThresholdNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::vector<Node*> VXThresholdNode::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&comparision_node);
	return subnodes;
}
std::string VXThresholdNode::generateClassDefinition()
{
	std::string s = comparision_node.generateClassDefinition();
	return s;
}
std::string VXThresholdNode::generateNodeCall()
{
	std::string s = comparision_node.generateNodeCall();
	return s;
}
void VXThresholdNode::build()
{
	comparision_node.in = in;
	comparision_node.out = out;

	if (threshold->threshold_type == VX_THRESHOLD_TYPE_BINARY)
	{
		ImageComparision ic;
		ic.comp_op = ">";
		ic.value = std::to_string(threshold->value);

		comparision_node.comparision.emplace_back(ic);


		comparision_node.true_value = threshold->true_value;
		comparision_node.false_value = threshold->false_value;
	}
	else
	{
		ImageComparision ic;
		ic.comp_op = ">";
		ic.value = std::to_string(threshold->upper);

		ImageComparision ic2;
		ic2.comp_op = "<";
		ic2.value = std::to_string(threshold->lower);

		comparision_node.comparision.emplace_back(ic);
		comparision_node.chaining_operators.emplace_back("||");
		comparision_node.comparision.emplace_back(ic2);

		comparision_node.true_value = threshold->false_value;
		comparision_node.false_value = threshold->true_value;
	}

	comparision_node.build();
}




Sobel3x3Node::Sobel3x3Node()
{
	node_name = "Sobel 3x3";
}
std::vector<Object *> Sobel3x3Node::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	return used_objects;
}
std::vector<Object *> Sobel3x3Node::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out_x);
	used_objects.emplace_back(out_y);
	return used_objects;
}
std::vector<Node*> Sobel3x3Node::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&sobel_x);
	subnodes.push_back(&sobel_y);
	return subnodes;
}
std::string Sobel3x3Node::generateClassDefinition()
{
	std::string s = sobel_x.generateClassDefinition();
	s += "\n" + sobel_y.generateClassDefinition();
	return s;
}
std::string Sobel3x3Node::generateNodeCall()
{
	std::string s = sobel_x.generateNodeCall();
	s += "\n" + sobel_y.generateNodeCall();
	return s;
}
void Sobel3x3Node::build()
{
	sobel_x.matrix.dim[0] = sobel_x.matrix.dim[1] = 3;
	sobel_y.matrix.dim[0] = sobel_y.matrix.dim[1] = 3;

	sobel_x.matrix.mask = {-1,  0,  1,
					-2,  0,  2,
					-1,  0,  1};

	sobel_y.matrix.mask = {-1, -2, -1,
					 0,  0,  0,
					 1,  2,  1};

	uint32_t one = 1;
	Scalar* x_normalization = new Scalar(VX_TYPE_INT32, &one);
	Scalar* y_normalization = new Scalar(VX_TYPE_INT32, &one);
	sobel_x.normalization.reset(x_normalization);
	sobel_y.normalization.reset(y_normalization);

	sobel_x.in = in;
	sobel_x.out = out_x;
	sobel_y.in = in;
	sobel_y.out = out_y;

	sobel_x.build();
	sobel_y.build();
}

Add3_3::Add3_3()
{
	node_name = "Add 3x3";
}
std::vector<Object *> Add3_3::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	return used_objects;
}
std::vector<Object *> Add3_3::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::vector<Node*> Add3_3::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&add);
	return subnodes;
}
std::string Add3_3::generateClassDefinition()
{
	std::string s = generator::node_generator(&add, generator::Type::Definition);
	return s;
}
std::string Add3_3::generateNodeCall()
{
	std::string s = generator::node_generator(&add, generator::Type::Call);
	return s;
}
void Add3_3::build()
{
	add.matrix.dim[0] = add.matrix.dim[1] = 3;

	add.matrix.mask = {1, 1, 1,
				1, 1, 1,
				1, 1, 1};

	uint32_t one = 1;
	Scalar* normalization = new Scalar(VX_TYPE_INT32, &one);
	add.normalization.reset(normalization);

	add.in = in;
	add.out = out;

	add.build();
}




std::vector<Object *> HarrisCorners::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	return used_objects;
}
std::vector<Object *> HarrisCorners::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(&Vc);
	used_objects.emplace_back(corners);
	return used_objects;
}
std::vector<Node*> HarrisCorners::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&sobel);
	subnodes.push_back(&sobel_x_norm);
	subnodes.push_back(&sobel_y_norm);
	subnodes.push_back(&sobel_x_square);
	subnodes.push_back(&sobel_y_square);
	subnodes.push_back(&sobel_x_y);
	subnodes.push_back(&gx_square_A);
	subnodes.push_back(&gy_square_A);
	subnodes.push_back(&gx_gy_A);

	subnodes.push_back(&trace_add);
	subnodes.push_back(&gx_A_gy_A);
	subnodes.push_back(&gx_gy_A_square);
	subnodes.push_back(&det_kernel);
	subnodes.push_back(&trace_A_square_kernel);
	subnodes.push_back(&trace_A_square_k_kernel);
	subnodes.push_back(&Mc_kernel);
	return subnodes;
}
std::string HarrisCorners::generateClassDefinition()
{
	std::string s = sobel.generateClassDefinition();
	s += "\n" + sobel_x_norm.generateClassDefinition();
	s += "\n" + sobel_y_norm.generateClassDefinition();
	s += "\n" + sobel_x_square.generateClassDefinition();
	s += "\n" + sobel_y_square.generateClassDefinition();
	s += "\n" + sobel_x_y.generateClassDefinition();
	s += "\n" + gx_square_A.generateClassDefinition();
	s += "\n" + gy_square_A.generateClassDefinition();
	s += "\n" + gx_gy_A.generateClassDefinition();
	s += "\n" + trace_add.generateClassDefinition();
	s += "\n" + gx_A_gy_A.generateClassDefinition();
	s += "\n" + gx_gy_A_square.generateClassDefinition();
	s += "\n" + det_kernel.generateClassDefinition();
	s += "\n" + trace_A_square_kernel.generateClassDefinition();
	s += "\n" + trace_A_square_k_kernel.generateClassDefinition();
	s += "\n" + Mc_kernel.generateClassDefinition();
	s += "\n" + generator::node_generator(this, generator::Type::Definition);
	return s;
}
std::string HarrisCorners::generateNodeCall()
{
	std::string s = sobel.generateNodeCall();
	s += "\n" + sobel_x_norm.generateNodeCall();
	s += "\n" + sobel_y_norm.generateNodeCall();
	s += "\n" + sobel_x_square.generateNodeCall();
	s += "\n" + sobel_y_square.generateNodeCall();
	s += "\n" + sobel_x_y.generateNodeCall();
	s += "\n" + gx_square_A.generateNodeCall();
	s += "\n" + gy_square_A.generateNodeCall();
	s += "\n" + gx_gy_A.generateNodeCall();
	s += "\n" + trace_add.generateNodeCall();
	s += "\n" + gx_A_gy_A.generateNodeCall();
	s += "\n" + gx_gy_A_square.generateNodeCall();
	s += "\n" + det_kernel.generateNodeCall();
	s += "\n" + trace_A_square_kernel.generateNodeCall();
	s += "\n" + trace_A_square_k_kernel.generateNodeCall();
	s += "\n" + Mc_kernel.generateNodeCall();
	s += "\n" + generator::node_generator(this, generator::Type::Call);
	return s;
}
void HarrisCorners::build()
{
	sobel.in = in;
	sobel.out_x = &sob_x;
	sobel.out_y = &sob_y;

	sobel_x_norm.in = &sob_x;
	sobel_x_norm.out = &Gx;
	sobel_y_norm.in = &sob_y;
	sobel_y_norm.out = &Gy;
	sobel_x_norm.scalar = sobel_y_norm.scalar = 1.f / (4 * 255 * 3/*b*/); // For 3x3 Sobel

	sobel_x_square.in_1 = &Gx;
	sobel_x_square.in_2 = &Gx;
	sobel_x_square.out = &square_Gx;
	sobel_y_square.in_1 = &Gy;
	sobel_y_square.in_2 = &Gy;
	sobel_y_square.out = &square_Gy;

	sobel_x_y.in_1 = &Gx;
	sobel_x_y.in_2 = &Gy;
	sobel_x_y.out = &det_A_mul_Gx_Gy;

	gx_square_A.in = &square_Gx;
	gx_square_A.out = &square_Gx_sum;
	gy_square_A.in = &square_Gy;
	gy_square_A.out = &square_Gy_sum;
	gx_gy_A.in = &det_A_mul_Gx_Gy;
	gx_gy_A.out = &det_A_mul_Gx_Gy_sum;

	trace_add.in_1 = &square_Gx_sum;
	trace_add.in_2 = &square_Gy_sum;
	trace_add.out = &trace_A;


	gx_A_gy_A.in_1 = &square_Gx_sum;
	gx_A_gy_A.in_2 = &square_Gy_sum;
	gx_A_gy_A.out = &det_A_minuend;

	gx_gy_A_square.in_1 = &det_A_mul_Gx_Gy_sum;
	gx_gy_A_square.in_2 = &det_A_mul_Gx_Gy_sum;
	gx_gy_A_square.out = &det_A_subtrahend;

	det_kernel.in_1 = &det_A_minuend;
	det_kernel.in_2 = &det_A_subtrahend;
	det_kernel.out = &det_A;

	trace_A_square_kernel.in_1 = &trace_A;
	trace_A_square_kernel.in_2 = &trace_A;
	trace_A_square_kernel.out = &trace_A_square;

	trace_A_square_k_kernel.in = &trace_A_square;
	trace_A_square_k_kernel.scalar = sensitivity->f32;
	trace_A_square_k_kernel.out = &trace_A_square_k;

	Mc_kernel.in_1 = &det_A;
	Mc_kernel.in_2 = &trace_A_square_k;
	Mc_kernel.out = &Mc;



	sobel.build();
	sobel_x_norm.build();
	sobel_y_norm.build();
	sobel_x_square.build();
	sobel_y_square.build();
	sobel_x_y.build();
	gx_square_A.build();
	gy_square_A.build();
	gx_gy_A.build();
	trace_add.build();
	gx_A_gy_A.build();
	gx_gy_A_square.build();
	det_kernel.build();
	trace_A_square_kernel.build();
	trace_A_square_k_kernel.build();
	Mc_kernel.build();
}



SquareNode::SquareNode()
{
	node_name = "Square";
}
std::vector<Object *> SquareNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	return used_objects;
}
std::vector<Object *> SquareNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::vector<Node*> SquareNode::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&mul_node);
	return subnodes;
}
std::string SquareNode::generateClassDefinition()
{
	std::string s = mul_node.generateClassDefinition();
	return s;
}
std::string SquareNode::generateNodeCall()
{
	std::string s = mul_node.generateNodeCall();
	return s;
}
void SquareNode::build()
{
	mul_node.in_1 = in;
	mul_node.in_2 = in;
	mul_node.out = out;

	mul_node.build();
}


std::vector<Object *> SimplePoint::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in_1);
	used_objects.emplace_back(in_2);
	return used_objects;
}
std::vector<Object *> SimplePoint::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::string SimplePoint::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string SimplePoint::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

SaturateNode::SaturateNode()
{
	node_name = "Saturate";
}
std::vector<Object *> SaturateNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	return used_objects;
}
std::vector<Object *> SaturateNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::string SaturateNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string SaturateNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

UnaryFunctionNode::UnaryFunctionNode()
{
	node_name = "General unary Function";
}
std::vector<Object *> UnaryFunctionNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	return used_objects;
}
std::vector<Object *> UnaryFunctionNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::string UnaryFunctionNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string UnaryFunctionNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

SqrtNode::SqrtNode()
{
	node_name = "Sqrt";
}
std::vector<Object *> SqrtNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	return used_objects;
}
std::vector<Object *> SqrtNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::vector<Node*> SqrtNode::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&function_node);
	return subnodes;
}
std::string SqrtNode::generateClassDefinition()
{
	std::string s = function_node.generateClassDefinition();
	return s;
}
std::string SqrtNode::generateNodeCall()
{
	std::string s = function_node.generateNodeCall();
	return s;
}
void SqrtNode::build()
{
	function_node.in = in;
	function_node.out = out;
	function_node.function = "sqrt";

	function_node.build();
}

AbsNode::AbsNode()
{
	node_name = "Abs";
}
std::vector<Object *> AbsNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	return used_objects;
}
std::vector<Object *> AbsNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::vector<Node*> AbsNode::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&function_node);
	return subnodes;
}
std::string AbsNode::generateClassDefinition()
{
	std::string s = function_node.generateClassDefinition();
	return s;
}
std::string AbsNode::generateNodeCall()
{
	std::string s = function_node.generateNodeCall();
	return s;
}
void AbsNode::build()
{
	function_node.in = in;
	function_node.out = out;
	function_node.function = "abs";

	function_node.build();
}

Atan2Node::Atan2Node()
{
	node_name = "Atan2";
}
std::vector<Object *> Atan2Node::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	return used_objects;
}
std::vector<Object *> Atan2Node::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::vector<Node*> Atan2Node::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&function_node);
	return subnodes;
}
std::string Atan2Node::generateClassDefinition()
{
	std::string s = function_node.generateClassDefinition();
	return s;
}
std::string Atan2Node::generateNodeCall()
{
	std::string s = function_node.generateNodeCall();
	return s;
}
void Atan2Node::build()
{
	function_node.in = in;
	function_node.out = out;
	function_node.function = "atan2";

	function_node.build();
}


AbsDiffNode::AbsDiffNode()
{
	node_name = "Absolute Difference";
}
std::vector<Object *> AbsDiffNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in_1);
	used_objects.emplace_back(in_2);
	return used_objects;
}
std::vector<Object *> AbsDiffNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::vector<Node*> AbsDiffNode::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&diff_node);
	subnodes.push_back(&abs_node);
	if (saturate)
		subnodes.push_back(&saturate_node);
	return subnodes;
}
std::string AbsDiffNode::generateClassDefinition()
{
	std::string s = diff_node.generateClassDefinition();
	s += "\n" + abs_node.generateClassDefinition();
	if (saturate)
		s += "\n" + saturate_node.generateClassDefinition();
	return s;
}
std::string AbsDiffNode::generateNodeCall()
{
	std::string s = diff_node.generateNodeCall();
	s += "\n" + abs_node.generateNodeCall();
	if (saturate)
		s += "\n" + saturate_node.generateNodeCall();
	return s;
}
void AbsDiffNode::build()
{
	//saturate_node is only used when input is S16
	saturate = in_1->col == VX_DF_IMAGE_S16 || in_2->col == VX_DF_IMAGE_S16;

	diff_node.in_1 = in_1;
	diff_node.in_2 = in_2;
	diff_image.reset(new Image(in_1->w, in_2->h, VX_DF_IMAGE_S32));
	diff_node.out = diff_image.get();

	abs_node.in = diff_image.get();
	if (!saturate)
	{
		abs_node.out = out;
	}
	else
	{
		abs_image.reset(new Image(in_1->w, in_2->h, VX_DF_IMAGE_S32));
		abs_node.out = abs_image.get();

		saturate_node.in = abs_image.get();
		saturate_node.out = out;
	}

	diff_node.build();
	abs_node.build();
	if (saturate)
		saturate_node.build();
}

VXAddNode::VXAddNode()
{
	node_name = "VX Addition Node";
}
std::vector<Object *> VXAddNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in_1);
	used_objects.emplace_back(in_2);
	return used_objects;
}
std::vector<Object *> VXAddNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::vector<Node*> VXAddNode::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&add_node);
	if (policy == VX_CONVERT_POLICY_SATURATE)
		subnodes.push_back(&saturate_node);
	return subnodes;
}
std::string VXAddNode::generateClassDefinition()
{
	std::string s = add_node.generateClassDefinition();
	if (policy == VX_CONVERT_POLICY_SATURATE)
		s += "\n" + saturate_node.generateClassDefinition();
	return s;
}
std::string VXAddNode::generateNodeCall()
{
	std::string s = add_node.generateNodeCall();
	if (policy == VX_CONVERT_POLICY_SATURATE)
		s += "\n" + saturate_node.generateNodeCall();
	return s;
}
void VXAddNode::build()
{
	add_node.in_1 = in_1;
	add_node.in_2 = in_2;
	if (policy != VX_CONVERT_POLICY_SATURATE)
	{
		add_node.out = out;
	}
	else
	{
		add_image.reset(new Image(in_1->w, in_2->h, VX_DF_IMAGE_S32));
		add_node.out = add_image.get();

		saturate_node.in = add_image.get();
		saturate_node.out = out;
	}

	add_node.build();
	if (policy == VX_CONVERT_POLICY_SATURATE)
		saturate_node.build();
}

VXSubtractNode::VXSubtractNode()
{
	node_name = "VX Subtraction Node";
}
std::vector<Object *> VXSubtractNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in_1);
	used_objects.emplace_back(in_2);
	return used_objects;
}
std::vector<Object *> VXSubtractNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::vector<Node*> VXSubtractNode::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&diff_node);
	if (policy == VX_CONVERT_POLICY_SATURATE)
		subnodes.push_back(&saturate_node);
	return subnodes;
}
std::string VXSubtractNode::generateClassDefinition()
{
	std::string s = diff_node.generateClassDefinition();
	if (policy == VX_CONVERT_POLICY_SATURATE)
		s += "\n" + saturate_node.generateClassDefinition();
	return s;
}
std::string VXSubtractNode::generateNodeCall()
{
	std::string s = diff_node.generateNodeCall();
	if (policy == VX_CONVERT_POLICY_SATURATE)
		s += "\n" + saturate_node.generateNodeCall();
	return s;
}
void VXSubtractNode::build()
{
	diff_node.in_1 = in_1;
	diff_node.in_2 = in_2;
	if (policy != VX_CONVERT_POLICY_SATURATE)
	{
		diff_node.out = out;
	}
	else
	{
		diff_image.reset(new Image(in_1->w, in_2->h, VX_DF_IMAGE_S32));
		diff_node.out = diff_image.get();

		saturate_node.in = diff_image.get();
		saturate_node.out = out;
	}

	diff_node.build();
	if (policy == VX_CONVERT_POLICY_SATURATE)
		saturate_node.build();
}

MagnitudeNode::MagnitudeNode()
{
	node_name = "VX Magnitude";
}
std::vector<Object *> MagnitudeNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in_1);
	used_objects.emplace_back(in_2);
	return used_objects;
}
std::vector<Object *> MagnitudeNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::vector<Node*> MagnitudeNode::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&grad_x_square_node);
	subnodes.push_back(&grad_y_square_node);
	subnodes.push_back(&add_node);
	subnodes.push_back(&sqrt_node);
	subnodes.push_back(&saturate_node);
	return subnodes;
}
std::string MagnitudeNode::generateClassDefinition()
{
	std::string s = grad_x_square_node.generateClassDefinition();
	s += "\n" + grad_y_square_node.generateClassDefinition();
	s += "\n" + add_node.generateClassDefinition();
	s += "\n" + sqrt_node.generateClassDefinition();
	s += "\n" + saturate_node.generateClassDefinition();
	return s;
}
std::string MagnitudeNode::generateNodeCall()
{
	std::string s = grad_x_square_node.generateNodeCall();
	s += "\n" + grad_y_square_node.generateNodeCall();
	s += "\n" + add_node.generateNodeCall();
	s += "\n" + sqrt_node.generateNodeCall();
	s += "\n" + saturate_node.generateNodeCall();
	return s;
}
void MagnitudeNode::build()
{
	grad_x_square_node.in = in_1;
	grad_x_square_image.reset(new Image(in_1->w, in_1->h, VX_DF_IMAGE_S32));
	grad_x_square_node.out = grad_x_square_image.get();

	grad_y_square_node.in = in_2;
	grad_y_square_image.reset(new Image(in_1->w, in_1->h, VX_DF_IMAGE_S32));
	grad_y_square_node.out = grad_y_square_image.get();

	add_node.in_1 = grad_x_square_image.get();
	add_node.in_2 = grad_y_square_image.get();
	add_image.reset(new Image(in_1->w, in_1->h, VX_DF_IMAGE_S32));
	add_node.out = add_image.get();

	sqrt_node.in = grad_x_square_image.get();
	sqrt_image.reset(new Image(in_1->w, in_2->h, VX_DF_IMAGE_S32));
	sqrt_node.out = sqrt_image.get();

	saturate_node.in = sqrt_image.get();
	saturate_node.out = out;


	grad_x_square_node.build();
	grad_y_square_node.build();
	add_node.build();
	sqrt_node.build();
	saturate_node.build();
}

PhaseNode::PhaseNode()
{
	node_name = "VX Phase";
}
std::vector<Object *> PhaseNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in_1);
	used_objects.emplace_back(in_2);
	return used_objects;
}
std::vector<Object *> PhaseNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::vector<Node*> PhaseNode::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&div_node);
	subnodes.push_back(&atan2_node);
	subnodes.push_back(&mapping_node);
	return subnodes;
}
std::string PhaseNode::generateClassDefinition()
{
	std::string s = div_node.generateClassDefinition();
	s += "\n" + atan2_node.generateClassDefinition();
	s += "\n" + mapping_node.generateClassDefinition();
	return s;
}
std::string PhaseNode::generateNodeCall()
{
	std::string s = div_node.generateNodeCall();
	s += "\n" + atan2_node.generateNodeCall();
	s += "\n" + mapping_node.generateNodeCall();
	return s;
}
void PhaseNode::build()
{
	div_node.in_1 = in_1;
	div_node.in_2 = in_2;
	div_image.reset(new Image(in_1->w, in_2->h, VX_TYPE_FLOAT32));
	div_node.out = div_image.get();

	atan2_node.in = div_image.get();
	atan2_image.reset(new Image(in_1->w, in_2->h, VX_TYPE_FLOAT32));
	atan2_node.out = atan2_image.get();

	mapping_node.in = atan2_image.get();
	mapping_node.scalar = 255.f / (M_2_PIf32);
	mapping_node.out = out;

	div_node.build();
	atan2_node.build();
	mapping_node.build();
}

VXMultiplyNode::VXMultiplyNode()
{
	node_name = "VX Multiplication";
}
std::vector<Object *> VXMultiplyNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in_1);
	used_objects.emplace_back(in_2);
	used_objects.emplace_back(scalar);
	return used_objects;
}
std::vector<Object *> VXMultiplyNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::vector<Node*> VXMultiplyNode::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&mul_node);
	subnodes.push_back(&mul_scalar_node);
	if (overflow_policy == VX_CONVERT_POLICY_SATURATE)
		subnodes.push_back(&saturate_node);
	return subnodes;
}
std::string VXMultiplyNode::generateClassDefinition()
{
	std::string s = mul_node.generateClassDefinition();
	s += "\n" + mul_scalar_node.generateClassDefinition();
	if (overflow_policy == VX_CONVERT_POLICY_SATURATE)
		s += "\n" + saturate_node.generateClassDefinition();
	return s;
}
std::string VXMultiplyNode::generateNodeCall()
{
	std::string s = mul_node.generateNodeCall();
	s += "\n" + mul_scalar_node.generateNodeCall();
	if (overflow_policy == VX_CONVERT_POLICY_SATURATE)
		s += "\n" + saturate_node.generateNodeCall();
	return s;
}
void VXMultiplyNode::build()
{
	mul_node.in_1 = in_1;
	mul_node.in_2 = in_2;
	mul_image.reset(new Image(in_1->w, in_2->h, VX_DF_IMAGE_S32));
	mul_node.out = mul_image.get();

	mul_scalar_node.out = mul_image.get();
	mul_scalar_node.scalar = scalar->f32;
	if (overflow_policy != VX_CONVERT_POLICY_SATURATE)
	{
		mul_scalar_node.out = out;
	}
	else
	{
		mul_scalar_image.reset(new Image(in_1->w, in_2->h, VX_DF_IMAGE_S32));
		mul_scalar_node.out = mul_scalar_image.get();

		saturate_node.in = mul_scalar_image.get();
		saturate_node.out = out;
	}

	mul_node.build();
	mul_scalar_node.build();
	if (overflow_policy == VX_CONVERT_POLICY_SATURATE)
		saturate_node.build();
}

VXAccumulateNode::VXAccumulateNode()
{
	node_name = "VX Accumulation";
}
std::vector<Object *> VXAccumulateNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	used_objects.emplace_back(in_out);
	return used_objects;
}
std::vector<Object *> VXAccumulateNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in_out);
	return used_objects;
}
std::vector<Node*> VXAccumulateNode::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&add_node);
	subnodes.push_back(&saturate_node);
	return subnodes;
}
std::string VXAccumulateNode::generateClassDefinition()
{
	std::string s = add_node.generateClassDefinition();
	s += "\n" + saturate_node.generateClassDefinition();
	return s;
}
std::string VXAccumulateNode::generateNodeCall()
{
	std::string s = add_node.generateNodeCall();
	s += "\n" + saturate_node.generateNodeCall();
	return s;
}
void VXAccumulateNode::build()
{
	add_node.in_1 = in;
	add_node.in_2 = in_out;
	add_image.reset(new Image(in->w, in->h, VX_DF_IMAGE_S32));
	add_node.out = add_image.get();

	saturate_node.in = add_image.get();
	saturate_node.out = in_out;

	add_node.build();
	saturate_node.build();
}

VXAccumulateSquareNode::VXAccumulateSquareNode()
{
	node_name = "VX Squared Accumulation";
}
std::vector<Object *> VXAccumulateSquareNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	used_objects.emplace_back(in_out);
	used_objects.emplace_back(shift);
	return used_objects;
}
std::vector<Object *> VXAccumulateSquareNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in_out);
	return used_objects;
}
std::vector<Node*> VXAccumulateSquareNode::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&square_node);
	subnodes.push_back(&depth_node);
	subnodes.push_back(&add_node);
	subnodes.push_back(&saturate_node);
	return subnodes;
}
std::string VXAccumulateSquareNode::generateClassDefinition()
{
	std::string s = square_node.generateClassDefinition();
	s += "\n" + depth_node.generateClassDefinition();
	s += "\n" + add_node.generateClassDefinition();
	s += "\n" + saturate_node.generateClassDefinition();
	return s;
}
std::string VXAccumulateSquareNode::generateNodeCall()
{
	std::string s = add_node.generateNodeCall();
	s += "\n" + depth_node.generateNodeCall();
	s += "\n" + add_node.generateNodeCall();
	s += "\n" + saturate_node.generateNodeCall();
	return s;
}
void VXAccumulateSquareNode::build()
{
	square_node.in = in;
	square_image.reset(new Image(in->w, in->h, VX_DF_IMAGE_S16));
	square_node.out = square_image.get();

	depth_node.in = in;
	depth_image.reset(new Image(in->w, in->h, VX_DF_IMAGE_S16));
	depth_node.out = square_image.get();
	depth_node.policy = VX_CONVERT_POLICY_WRAP;
	int32_t signed_shift = shift->ui32;
	depth_scalar.reset(new Scalar(VX_TYPE_UINT32, &signed_shift));
	depth_node.shift = depth_scalar.get();


	add_node.in_1 = square_image.get();
	add_node.in_2 = in_out;
	add_image.reset(new Image(in->w, in->h, VX_DF_IMAGE_S16));
	add_node.out = add_image.get();

	saturate_node.in = add_image.get();
	saturate_node.out = in_out;

	square_node.build();
	depth_node.build();
	add_node.build();
	saturate_node.build();
}

VXAccumulateWeightedNode::VXAccumulateWeightedNode()
{
	node_name = "VX Weighted Accumulation";
}
std::vector<Object *> VXAccumulateWeightedNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	used_objects.emplace_back(in_out);
	used_objects.emplace_back(alpha);
	return used_objects;
}
std::vector<Object *> VXAccumulateWeightedNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in_out);
	return used_objects;
}
std::vector<Node*> VXAccumulateWeightedNode::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&mul_scalar_left_node);
	subnodes.push_back(&mul_scalar_right_node);
	subnodes.push_back(&add_node);
	return subnodes;
}
std::string VXAccumulateWeightedNode::generateClassDefinition()
{
	std::string s = mul_scalar_left_node.generateClassDefinition();
	s += "\n" + mul_scalar_right_node.generateClassDefinition();
	s += "\n" + add_node.generateClassDefinition();
	return s;
}
std::string VXAccumulateWeightedNode::generateNodeCall()
{
	std::string s = mul_scalar_left_node.generateNodeCall();
	s += "\n" + mul_scalar_right_node.generateNodeCall();
	s += "\n" + add_node.generateNodeCall();
	return s;
}
void VXAccumulateWeightedNode::build()
{
	mul_scalar_left_node.in = in;
	mul_scalar_left_image.reset(new Image(in->w, in->h, VX_TYPE_FLOAT32));
	mul_scalar_left_node.out = mul_scalar_left_image.get();
	mul_scalar_left_node.scalar = 1.f - alpha->f32;

	mul_scalar_right_node.in = in_out;
	mul_scalar_right_image.reset(new Image(in->w, in->h, VX_TYPE_FLOAT32));
	mul_scalar_right_node.out = mul_scalar_right_image.get();
	mul_scalar_right_node.scalar = alpha->f32;

	add_node.in_1 = mul_scalar_left_image.get();
	add_node.in_2 = mul_scalar_right_image.get();
	add_image.reset(new Image(in->w, in->h, VX_DF_IMAGE_S16));
	add_node.out = in_out;


	mul_scalar_left_node.build();
	mul_scalar_right_node.build();
	add_node.build();
}

VXChannelExtractNode::VXChannelExtractNode()
{
	node_name = "VX Channel Extraction";
}
std::vector<Object *> VXChannelExtractNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	return used_objects;
}
std::vector<Object *> VXChannelExtractNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::string VXChannelExtractNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string VXChannelExtractNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

VXConvolveNode::VXConvolveNode()
{
	node_name = "VX Convolution";
}
std::vector<Object *> VXConvolveNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	used_objects.emplace_back(convolution);
	return used_objects;
}
std::vector<Object *> VXConvolveNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::vector<Node*> VXConvolveNode::get_subnodes()
{
	std::vector<Node*> subnodes;
	subnodes.push_back(&lin_mask_node);
	if (out->col == VX_DF_IMAGE_U8)
		subnodes.push_back(&saturate_node);
	return subnodes;
}
std::string VXConvolveNode::generateClassDefinition()
{
	std::string s = lin_mask_node.generateClassDefinition();
	if (out->col == VX_DF_IMAGE_U8)
		s += "\n" + saturate_node.generateClassDefinition();
	return s;
}
std::string VXConvolveNode::generateNodeCall()
{
	std::string s = lin_mask_node.generateNodeCall();
	if (out->col == VX_DF_IMAGE_U8)
		s += "\n" + saturate_node.generateNodeCall();
	return s;
}
void VXConvolveNode::build()
{
	lin_mask_node.in = in;
	lin_mask_node.matrix.dim[0] = convolution->rows;
	lin_mask_node.matrix.dim[1] = convolution->columns;
	lin_mask_node.matrix.mask.reserve(lin_mask_node.matrix.dim[0] * lin_mask_node.matrix.dim[1]);
	for (unsigned int i = 0; i < lin_mask_node.matrix.dim[0] * lin_mask_node.matrix.dim[1]; i++)
		lin_mask_node.matrix.mask[i] = convolution->coefficients[i];

	float one = 1.f / convolution->scale;
	Scalar* normalization = new Scalar(VX_TYPE_FLOAT32, &one);
	lin_mask_node.normalization.reset(normalization);

	if (out->col != VX_DF_IMAGE_U8)
	{
		lin_mask_node.out = out;
	}
	else
	{
		lin_mask_image.reset(new Image(in->w, in->h, VX_DF_IMAGE_S16));
		lin_mask_node.out = lin_mask_image.get();

		saturate_node.in = lin_mask_image.get();
		saturate_node.out = out;
	}
	lin_mask_node.build();
	saturate_node.build();
}

HipaccNode::HipaccNode()
{
	node_name = "Custom Hipacc";
}
std::vector<Object *> HipaccNode::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.insert(used_objects.end(), parameters.begin(), parameters.end());
	return used_objects;
}
std::vector<Object *> HipaccNode::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
std::string HipaccNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string HipaccNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

}

static std::string get_object_name(HipaVX::Object *object)
{
	std::string type;

	switch(object->type)
	{
	case VX_TYPE_IMAGE:
		type = "Image";
		break;
	case VX_TYPE_SCALAR:
		type = "Scalar";
		break;
	case VX_TYPE_MATRIX:
	case VX_TYPE_HIPAVX_MATRIX:
		type = "Matrix";
		break;
	case VX_TYPE_THRESHOLD:
		type = "Threshold";
		break;
	case VX_TYPE_CONVOLUTION:
		type = "Convolution";
		break;
	default:
		throw std::runtime_error("static std::string generate_object_name(HipaVX::Object *object):\n\tUnsupported objecttype");
	}

	return type + "_" + std::to_string(object->my_id);
}
void vxDrawDotGraph(vx_graph graph, std::string filename, vx_uint32 node_depth)
{
	if (node_depth != 0)
		throw std::runtime_error("Currently only node_depth of 0 is supported");
	std::vector<HipaVX::Node*> nodes = graph->graph;
	std::vector<HipaVX::Object*> objects;

	std::string content;

	std::string edges;
	for(auto node: nodes)
	{
		auto inputs = node->get_inputs();
		auto outputs = node->get_outputs();

		for(auto input: inputs)
		{
			edges += "\tID_" + std::to_string(input->my_id) + " -> ID_" + std::to_string(node->my_id) + ";\n";
		}
		for(auto output: outputs)
		{
			edges += "\tID_" + std::to_string(node->my_id) + " -> ID_" + std::to_string(output->my_id) + ";\n";
		}

		objects.insert(objects.end(), inputs.begin(), inputs.end());
		objects.insert(objects.end(), outputs.begin(), outputs.end());
	}

	std::sort(objects.begin(), objects.end());
	objects.erase(std::unique(objects.begin(), objects.end()), objects.end());


	std::string node_definitions;
	for(auto node: nodes)
		node_definitions += "\tID_" + std::to_string(node->my_id) + " [label=\"" + node->get_name() + "\", color=green];\n";
	for(auto object: objects)
		node_definitions += "\tID_" + std::to_string(object->my_id) + " [label=\"" + get_object_name(object) + "\", shape=box, color=blue];\n";

	content += node_definitions;
	content += edges;
	std::ofstream dot_writer(filename);
	dot_writer << "digraph graphname {\n" << content << "}";
}
