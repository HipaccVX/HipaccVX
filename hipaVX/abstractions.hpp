#include <iostream>
#include <vector>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <memory>
#include <string>
#include <algorithm>
#include <memory>

#include "../VX/vx.h"
#include "domVX_types.hpp"
#include "kernels/domVX_kernels.hpp"
#include "ast.hpp"
//#include "hipacc_gen/ast_gen.hpp"

#pragma once

using std::string;

namespace DomVX
{

// TODO: This header should be independent from the vx_types
//        move Image definition to here, and rename to Acc2D

enum class AbstractionType
{
    None,
    Reduce,
    Local,
    LocalOp,
    Map,
    LocalOperation,
    MapFindName
};

class AbstractionNode
{
public:
    int id;
    AbstractionType type = AbstractionType::None;
    AbstractionNode() {
        static int next_id = 0;
        id = next_id++;
    }
    std::vector<std::shared_ptr<ast4vx::Node>> subnodes;
    std::shared_ptr<ast4vx::Node>& operator[](unsigned int index) {
        return subnodes[index]; 
    }
    std::shared_ptr<ast4vx::Node> operator[](unsigned int index) const {
        return subnodes[index]; 
    }

    virtual ~AbstractionNode() = default;
};

class Datum
{
public:
    HipaVX::Image *im;
    int x, y;

    Datum(HipaVX::Image *image, int X, int Y)
        :im(image), x(X), y(Y)
    {}
};

class Map: public AbstractionNode
{
    std::shared_ptr<ast4vx::Statements> function;
public:
    Map()
    {
        type = AbstractionType::Map;
    }

    Map(std::shared_ptr<ast4vx::Statements> s)
    {
        type = AbstractionType::Map;
        set_statements(s);
    }

    std::vector<HipaVX::Image *> output_pixel_mappings;
    std::vector<HipaVX::Image *> input_pixel_mappings;

    void set_statements(std::shared_ptr<ast4vx::Statements> s)
    {
        function = s;
        output_pixel_mappings.resize(s->out_pixel_mappings.size());
        input_pixel_mappings.resize(s->in_pixel_mappings.size());
    }

    std::shared_ptr<ast4vx::Statements> get_statements()
    {
        return function;
    }

    void register_images(std::initializer_list<HipaVX::Image*> out, std::initializer_list<HipaVX::Image*> in)
    {
        register_output_images(out);
        register_input_images(in);
    }
    void register_input_images(std::initializer_list<HipaVX::Image*> images)
    {
        if (images.size() != input_pixel_mappings.size())
            throw std::runtime_error("void Map::register_input_images(): Initializer list has to be the same size as the input_accessors of the underlying statemens");

        input_pixel_mappings.clear();
        for(auto image: images)
            input_pixel_mappings.emplace_back(image);
    }
    void register_output_images(std::initializer_list<HipaVX::Image*> images)
    {
        if (images.size() != output_pixel_mappings.size())
            throw std::runtime_error("void Map::register_output_images(): Initializer list has to be the same size as the output_accessors of the underlying statemens");

        output_pixel_mappings.clear();
        for(auto image: images)
            output_pixel_mappings.emplace_back(image);
    }
};


class Reduce: public AbstractionNode
{
public:
    Reduce()
    {
        type = AbstractionType::Reduce;
    }
    HipaVX::Image* left_img = new HipaVX::Image(1, 1, VX_DF_IMAGE_U8);
    HipaVX::Image* right_img = new HipaVX::Image(1, 1, VX_DF_IMAGE_U8);
    HipaVX::Image* out_img = new HipaVX::Image(1, 1, VX_DF_IMAGE_U8);
    ast4vx::Statements function;

    // TODO: change the printer functions to determine the target and source pixels at the time of printing
    std::shared_ptr<ast4vx::Node> dleft = std::make_shared<ast4vx::CurrentPixelvalue>(std::make_shared<ast4vx::Image>(left_img));
    std::shared_ptr<ast4vx::Node> dright = std::make_shared<ast4vx::CurrentPixelvalue>(std::make_shared<ast4vx::Image>(right_img));
    std::shared_ptr<ast4vx::Node> dout = std::make_shared<ast4vx::CurrentPixelvalue>(std::make_shared<ast4vx::Image>(out_img));
};

class MapDesc: public AbstractionNode, public ast4vx::Variable
{

public:
    MapDesc()
    {
        AbstractionNode::type = AbstractionType::MapFindName;
        name = "MapFindName_" + std::to_string(AbstractionNode::id);
    }
    std::shared_ptr<MapDesc> successor;
    std::shared_ptr<Map> me;
    int x, y;
};

class LocalOperation: public AbstractionNode
{
public:
    std::vector<std::tuple<HipaVX::Image*, std::shared_ptr<ast4vx::WindowDescriptor>>> input_descriptor;
    std::vector<std::shared_ptr<ast4vx::WindowOperation>> operations;
    std::vector<std::vector<HipaVX::Image *>> operation_output_images;

public:

    LocalOperation()
    {
        type = AbstractionType::LocalOperation;
    }

    void set_input_window_desc(std::initializer_list<std::tuple<HipaVX::Image*, std::shared_ptr<ast4vx::WindowDescriptor>>> in_descriptors)
    {
        input_descriptor.clear();
        for (auto desc: in_descriptors)
            input_descriptor.emplace_back(desc);
    }

    void add_operation(std::shared_ptr<ast4vx::WindowOperation> op, std::initializer_list<HipaVX::Image*> out = {})
    {
        operations.emplace_back(op);
        operation_output_images.emplace_back(out);
    }
};

}


template <class ReturnType, class ParameterType>
class AbstractionsVisitor
{
public:
    virtual ReturnType visit(std::shared_ptr<DomVX::AbstractionNode> n, ParameterType p) = 0;
};

