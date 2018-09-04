#include "hipaVX_internal.hpp"


namespace HipaVX
{
Scalar::Scalar(vx_type_e t, const void *ptr)
	:type(t)
{
	switch(type)
	{
	case VX_TYPE_INT32:
		i32=*((vx_int32*) ptr);
	}
}

int Image::next_id = 0;
Image::Image(vx_uint32 width, vx_uint32 height, vx_df_image color)
	:my_id(next_id++), w(width), h(height), col(color)
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
	used_images.emplace_back(src);
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
	used_images.emplace_back(src);
	used_images.emplace_back(dst_x);
	used_images.emplace_back(dst_y);
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
	used_images.emplace_back(src);
	used_images.emplace_back(dst);
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

}


