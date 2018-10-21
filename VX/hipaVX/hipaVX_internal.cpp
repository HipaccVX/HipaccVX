#include "hipaVX_internal.hpp"


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
	used_images.clear();

	for (auto& node: graph)
	{
		node->build();
		auto used_images_node = node->get_used_images();
		used_images.insert(used_images.end(), used_images_node.cbegin(), used_images_node.cend());
	}

	std::sort(used_images.begin(), used_images.end());
	used_images.erase(std::unique(used_images.begin(), used_images.end()), used_images.end());

	built = true;
}

std::vector<Image *> WriteImageNode::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in);
	return used_images;
}
std::string WriteImageNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string WriteImageNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

std::vector<Image *> ConvertDepthNode::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in);
	used_images.emplace_back(out);
	return used_images;
}
std::string ConvertDepthNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string ConvertDepthNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

std::vector<Image *> NotNode::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in);
	used_images.emplace_back(out);
	return used_images;
}
std::string NotNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string NotNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

std::vector<Image *> Dilate::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in);
	used_images.emplace_back(out);
	return used_images;
}
std::string Dilate::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string Dilate::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

std::vector<Image *> Erode::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in);
	used_images.emplace_back(out);
	return used_images;
}
std::string Erode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string Erode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}



std::vector<Image *> VXThresholdNode::get_used_images()
{
	std::vector<Image*> used_images;
	auto x = comparision_node.get_used_images();
	std::copy(x.begin(), x.end(), std::back_inserter(used_images));
	return used_images;
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

	if (threshold->type == VX_THRESHOLD_TYPE_BINARY)
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



std::vector<Image *> VXChannelExtractNode::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in);
	used_images.emplace_back(out);
	return used_images;
}
std::string VXChannelExtractNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string VXChannelExtractNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}




std::vector<Image *> Sobel3x3Node::get_used_images()
{
	std::vector<Image*> used_images;
	auto x = sobel_x.get_used_images();
	auto y = sobel_y.get_used_images();
	std::copy(x.begin(), x.end(), std::back_inserter(used_images));
	std::copy(y.begin(), y.end(), std::back_inserter(used_images));
	return used_images;
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
	sobel_x.dim[0] = sobel_x.dim[1] = sobel_y.dim[0] = sobel_y.dim[1] = 3;

	sobel_x.mask = {-1,  0,  1,
					-2,  0,  2,
					-1,  0,  1};

	sobel_y.mask = {-1, -2, -1,
					 0,  0,  0,
					 1,  2,  1};

	sobel_x.normalization = sobel_y.normalization = 1;

	sobel_x.in = in;
	sobel_x.out = out_x;
	sobel_y.in = in;
	sobel_y.out = out_y;

	sobel_x.build();
	sobel_y.build();
}


std::vector<Image *> Add3_3::get_used_images()
{
	std::vector<Image*> used_images;
	auto a = add.get_used_images();
	std::copy(a.begin(), a.end(), std::back_inserter(used_images));
	return used_images;
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
	add.dim[0] = add.dim[1] = 3;

	add.mask = {1, 1, 1,
				1, 1, 1,
				1, 1, 1};

	add.normalization = 1;

	add.in = in;
	add.out = out;

	add.build();
}




std::vector<Image *> HarrisCorners::get_used_images()
{
	std::vector<Image*> used_images;
	auto a = sobel.get_used_images();
	auto b = sobel_x_norm.get_used_images();
	auto c = sobel_y_norm.get_used_images();
	auto d = sobel_x_square.get_used_images();
	auto e = sobel_y_square.get_used_images();
	auto f = sobel_x_y.get_used_images();
	auto g = gx_square_A.get_used_images();
	auto h = gy_square_A.get_used_images();
	auto i = gx_gy_A.get_used_images();
	auto j = trace_add.get_used_images();
	auto k = gx_A_gy_A.get_used_images();
	auto l = gx_gy_A_square.get_used_images();
	auto m = det_kernel.get_used_images();
	auto n = trace_A_square_kernel.get_used_images();
	auto o = trace_A_square_k_kernel.get_used_images();
	auto p = Mc_kernel.get_used_images();
	std::copy(a.begin(), a.end(), std::back_inserter(used_images));
	std::copy(b.begin(), b.end(), std::back_inserter(used_images));
	std::copy(c.begin(), c.end(), std::back_inserter(used_images));
	std::copy(d.begin(), d.end(), std::back_inserter(used_images));
	std::copy(e.begin(), e.end(), std::back_inserter(used_images));
	std::copy(f.begin(), f.end(), std::back_inserter(used_images));
	std::copy(g.begin(), g.end(), std::back_inserter(used_images));
	std::copy(h.begin(), h.end(), std::back_inserter(used_images));
	std::copy(i.begin(), i.end(), std::back_inserter(used_images));
	std::copy(j.begin(), j.end(), std::back_inserter(used_images));
	std::copy(k.begin(), k.end(), std::back_inserter(used_images));
	std::copy(l.begin(), l.end(), std::back_inserter(used_images));
	std::copy(m.begin(), m.end(), std::back_inserter(used_images));
	std::copy(n.begin(), n.end(), std::back_inserter(used_images));
	std::copy(o.begin(), o.end(), std::back_inserter(used_images));
	std::copy(p.begin(), p.end(), std::back_inserter(used_images));
	used_images.push_back(&Vc);
	return used_images;
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




std::vector<Image *> SquareNode::get_used_images()
{
	std::vector<Image*> used_images;
	auto a = mul_node.get_used_images();
	std::copy(a.begin(), a.end(), std::back_inserter(used_images));
	return used_images;
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


std::vector<Image *> SimplePoint::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in_1);
	used_images.emplace_back(in_2);
	used_images.emplace_back(out);
	return used_images;
}
std::string SimplePoint::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string SimplePoint::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

std::vector<Image *> SaturateNode::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in);
	used_images.emplace_back(out);
	return used_images;
}
std::string SaturateNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string SaturateNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

std::vector<Image *> UnaryFunctionNode::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in);
	used_images.emplace_back(out);
	return used_images;
}
std::string UnaryFunctionNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string UnaryFunctionNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}


std::vector<Image *> SqrtNode::get_used_images()
{
	std::vector<Image*> used_images;
	auto a = function_node.get_used_images();
	std::copy(a.begin(), a.end(), std::back_inserter(used_images));
	return used_images;
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

std::vector<Image *> AbsNode::get_used_images()
{
	std::vector<Image*> used_images;
	auto a = function_node.get_used_images();
	std::copy(a.begin(), a.end(), std::back_inserter(used_images));
	return used_images;
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

std::vector<Image *> Atan2Node::get_used_images()
{
	std::vector<Image*> used_images;
	auto a = function_node.get_used_images();
	std::copy(a.begin(), a.end(), std::back_inserter(used_images));
	return used_images;
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



std::vector<Image *> AbsDiffNode::get_used_images()
{
	std::vector<Image*> used_images;
	auto a = diff_node.get_used_images();
	auto b = abs_node.get_used_images();
	std::copy(a.begin(), a.end(), std::back_inserter(used_images));
	std::copy(b.begin(), b.end(), std::back_inserter(used_images));

	if (saturate)
	{
		auto c = saturate_node.get_used_images();
		std::copy(c.begin(), c.end(), std::back_inserter(used_images));
	}
	return used_images;
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

std::vector<Image *> AddNode::get_used_images()
{
	std::vector<Image*> used_images;
	auto a = add_node.get_used_images();
	std::copy(a.begin(), a.end(), std::back_inserter(used_images));

	if (policy == VX_CONVERT_POLICY_SATURATE)
	{
		auto c = saturate_node.get_used_images();
		std::copy(c.begin(), c.end(), std::back_inserter(used_images));
	}
	return used_images;
}
std::string AddNode::generateClassDefinition()
{
	std::string s = add_node.generateClassDefinition();
	if (policy == VX_CONVERT_POLICY_SATURATE)
		s += "\n" + saturate_node.generateClassDefinition();
	return s;
}
std::string AddNode::generateNodeCall()
{
	std::string s = add_node.generateNodeCall();
	if (policy == VX_CONVERT_POLICY_SATURATE)
		s += "\n" + saturate_node.generateNodeCall();
	return s;
}
void AddNode::build()
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

std::vector<Image *> SubtractNode::get_used_images()
{
	std::vector<Image*> used_images;
	auto a = diff_node.get_used_images();
	std::copy(a.begin(), a.end(), std::back_inserter(used_images));

	if (policy == VX_CONVERT_POLICY_SATURATE)
	{
		auto c = saturate_node.get_used_images();
		std::copy(c.begin(), c.end(), std::back_inserter(used_images));
	}
	return used_images;
}
std::string SubtractNode::generateClassDefinition()
{
	std::string s = diff_node.generateClassDefinition();
	if (policy == VX_CONVERT_POLICY_SATURATE)
		s += "\n" + saturate_node.generateClassDefinition();
	return s;
}
std::string SubtractNode::generateNodeCall()
{
	std::string s = diff_node.generateNodeCall();
	if (policy == VX_CONVERT_POLICY_SATURATE)
		s += "\n" + saturate_node.generateNodeCall();
	return s;
}
void SubtractNode::build()
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

std::vector<Image *> MagnitudeNode::get_used_images()
{
	std::vector<Image*> used_images;
	auto a = grad_x_square_node.get_used_images();
	auto b = grad_y_square_node.get_used_images();
	auto c = add_node.get_used_images();
	auto d = sqrt_node.get_used_images();
	auto e = saturate_node.get_used_images();
	std::copy(a.begin(), a.end(), std::back_inserter(used_images));
	std::copy(b.begin(), b.end(), std::back_inserter(used_images));
	std::copy(c.begin(), c.end(), std::back_inserter(used_images));
	std::copy(d.begin(), d.end(), std::back_inserter(used_images));
	std::copy(e.begin(), e.end(), std::back_inserter(used_images));
	return used_images;
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

std::vector<Image *> PhaseNode::get_used_images()
{
	std::vector<Image*> used_images;
	auto a = div_node.get_used_images();
	auto b = atan2_node.get_used_images();
	auto c = mapping_node.get_used_images();
	std::copy(a.begin(), a.end(), std::back_inserter(used_images));
	std::copy(b.begin(), b.end(), std::back_inserter(used_images));
	std::copy(c.begin(), c.end(), std::back_inserter(used_images));
	return used_images;
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

std::vector<Image *> VXMultiplyNode::get_used_images()
{
	std::vector<Image*> used_images;
	auto a = mul_node.get_used_images();
	std::copy(a.begin(), a.end(), std::back_inserter(used_images));
	auto b = mul_scalar_node.get_used_images();
	std::copy(b.begin(), b.end(), std::back_inserter(used_images));

	if (overflow_policy == VX_CONVERT_POLICY_SATURATE)
	{
		auto c = saturate_node.get_used_images();
		std::copy(c.begin(), c.end(), std::back_inserter(used_images));
	}
	return used_images;
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

std::vector<Image *> VXAccumulateNode::get_used_images()
{
	std::vector<Image*> used_images;
	auto a = add_node.get_used_images();
	auto c = saturate_node.get_used_images();
	std::copy(a.begin(), a.end(), std::back_inserter(used_images));
	std::copy(c.begin(), c.end(), std::back_inserter(used_images));
	return used_images;
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

std::vector<Image *> VXAccumulateSquareNode::get_used_images()
{
	std::vector<Image*> used_images;
	auto a = square_node.get_used_images();
	auto b = depth_node.get_used_images();
	auto c = add_node.get_used_images();
	auto d = saturate_node.get_used_images();
	std::copy(a.begin(), a.end(), std::back_inserter(used_images));
	std::copy(b.begin(), b.end(), std::back_inserter(used_images));
	std::copy(c.begin(), c.end(), std::back_inserter(used_images));
	std::copy(d.begin(), d.end(), std::back_inserter(used_images));
	return used_images;
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

std::vector<Image *> VXAccumulateWeightedNode::get_used_images()
{
	std::vector<Image*> used_images;
	auto a = mul_scalar_left_node.get_used_images();
	auto b = mul_scalar_right_node.get_used_images();
	auto c = add_node.get_used_images();
	std::copy(a.begin(), a.end(), std::back_inserter(used_images));
	std::copy(b.begin(), b.end(), std::back_inserter(used_images));
	std::copy(c.begin(), c.end(), std::back_inserter(used_images));
	return used_images;
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

std::vector<Image *> VXConvolveNode::get_used_images()
{
	std::vector<Image*> used_images;
	auto a = lin_mask_node.get_used_images();
	std::copy(a.begin(), a.end(), std::back_inserter(used_images));
	if (out->col == VX_DF_IMAGE_U8)
	{
		auto c = saturate_node.get_used_images();
		std::copy(c.begin(), c.end(), std::back_inserter(used_images));
	}
	return used_images;
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
	lin_mask_node.dim[0] = convolution->rows;
	lin_mask_node.dim[1] = convolution->columns;
	lin_mask_node.mask.reserve(lin_mask_node.dim[0] * lin_mask_node.dim[1]);
	for (unsigned int i = 0; i < lin_mask_node.dim[0] * lin_mask_node.dim[1]; i++)
		lin_mask_node.mask[i] = convolution->coefficients[i];
	lin_mask_node.normalization = 1.f / convolution->scale;

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

std::vector<Image *> HipaccNode::get_used_images()
{
	std::vector<Image *> images;
	for (auto param: parameters)
	{
		if (param->type == VX_TYPE_IMAGE)
		{
			Image *img = (Image*) param;
			images.push_back(img);
		}
	}
	return images;
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


