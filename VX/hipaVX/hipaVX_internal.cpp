#include "hipaVX_internal.hpp"


namespace HipaVX
{
Scalar::Scalar(vx_type_e t, const void *ptr)
	:type(t)
{
	switch(type)
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

int Image::next_id = 0;

Image::Image(vx_uint32 width, vx_uint32 height, vx_df_image color)
	:my_id(next_id++), w(width), h(height), col(color)
{
}

Array::Array(vx_enum item_type, vx_size cap, vx_size rows)
	:Image(rows, cap, VX_DF_IMAGE_S32), type(item_type), capacity(cap)
{
}

FileinputImage::FileinputImage(vx_uint32 width, vx_uint32 height, vx_df_image color, std::string filename)
	:Image(width, height, color), file(filename)
{
}

int Node::next_id = 0;
Node::Node()
	:my_id(next_id++)
{}

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

std::vector<Image *> Sobel3x3Node::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in);
	used_images.emplace_back(out_x);
	used_images.emplace_back(out_y);
	return used_images;
}
std::string Sobel3x3Node::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string Sobel3x3Node::generateNodeCall()
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

std::vector<Image *> MagnitudeNode::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(grad_x);
	used_images.emplace_back(grad_y);
	used_images.emplace_back(mag);
	return used_images;
}
std::string MagnitudeNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string MagnitudeNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

std::vector<Image *> AbsDiffNode::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in_1);
	used_images.emplace_back(in_2);
	used_images.emplace_back(out);
	return used_images;
}
std::string AbsDiffNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string AbsDiffNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

std::vector<Image *> AddNode::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in_1);
	used_images.emplace_back(in_2);
	used_images.emplace_back(out);
	return used_images;
}
std::string AddNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string AddNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

std::vector<Image *> SubtractNode::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in_1);
	used_images.emplace_back(in_2);
	used_images.emplace_back(out);
	return used_images;
}
std::string SubtractNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string SubtractNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

std::vector<Image *> AndNode::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in_1);
	used_images.emplace_back(in_2);
	used_images.emplace_back(out);
	return used_images;
}
std::string AndNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string AndNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

std::vector<Image *> XorNode::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in_1);
	used_images.emplace_back(in_2);
	used_images.emplace_back(out);
	return used_images;
}
std::string XorNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string XorNode::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

std::vector<Image *> OrNode::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in_1);
	used_images.emplace_back(in_2);
	used_images.emplace_back(out);
	return used_images;
}
std::string OrNode::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string OrNode::generateNodeCall()
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

std::vector<Image *> BoxFilter::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in);
	used_images.emplace_back(out);
	return used_images;
}
std::string BoxFilter::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string BoxFilter::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

std::vector<Image *> GaussianFilter::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in);
	used_images.emplace_back(out);
	return used_images;
}
std::string GaussianFilter::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string GaussianFilter::generateNodeCall()
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

std::vector<Image *> HarrisCorners::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in);
	used_images.emplace_back(&Gx);
	used_images.emplace_back(&Gy);
	used_images.emplace_back(&square_Gx);
	used_images.emplace_back(&square_Gy);
	used_images.emplace_back(&square_Gx_sum);
	used_images.emplace_back(&square_Gy_sum);
	used_images.emplace_back(&trace_A);
	used_images.emplace_back(&det_A_minuend);
	used_images.emplace_back(&det_A_mul_Gx_Gy);
	used_images.emplace_back(&det_A_mul_Gx_Gy_sum);
	used_images.emplace_back(&det_A_subtrahend);
	used_images.emplace_back(&det_A);
	used_images.emplace_back(&Mc);
	used_images.emplace_back(&Vc);

	used_images.emplace_back(corners);
	return used_images;
}
std::string HarrisCorners::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
std::string HarrisCorners::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}

std::vector<Image *> Sobel3_3::get_used_images()
{
	std::vector<Image*> used_images;
	auto x = sobel_x.get_used_images();
	auto y = sobel_y.get_used_images();
	std::copy(x.begin(), x.end(), std::back_inserter(used_images));
	std::copy(y.begin(), y.end(), std::back_inserter(used_images));
	return used_images;
}
std::string Sobel3_3::generateClassDefinition()
{
	std::string s = generator::node_generator(&sobel_x, generator::Type::Definition);
	s += "\n" + generator::node_generator(&sobel_y, generator::Type::Definition);
	return s;
}
std::string Sobel3_3::generateNodeCall()
{
	std::string s = generator::node_generator(&sobel_x, generator::Type::Call);
	s += "\n" + generator::node_generator(&sobel_y, generator::Type::Call);
	return s;
}
void Sobel3_3::build()
{
	sobel_x.build();
	sobel_y.build();

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
}


}


