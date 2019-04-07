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
#include <map>

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
    Domain,
    Map,
    LocalOperation,

    Mask,
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

class Domain: public AbstractionNode
{
public:

    unsigned int width, height;
    std::vector<std::vector<unsigned char>> domain;
    Domain(unsigned int x, unsigned int y, std::initializer_list<int> dom)
        :width(x), height(y)
    {
        type = AbstractionType::Domain;
        set_domain(dom);
    }

    void set_domain(std::initializer_list<int> dom)
    {
        if (dom.size() != height * width)
            throw std::runtime_error("Domain::set_domain: dom needs to have x * y elements");

        domain.clear();
        auto dom_it = dom.begin();
        for(unsigned int y = 0; y < height; y++)
        {
            domain.push_back(std::vector<unsigned char>());
            for(unsigned x = 0; x < width; x++)
            {
                domain[y].push_back((*dom_it != 0)?1:0);
                dom_it++;
            }
        }
    }
};

class Mask: public AbstractionNode
{
public:
    union mask_type
    {
        int32_t i;
        float f;
        mask_type(int32_t i) :i(i){}
        mask_type(float f) :f(f){}
    };

    unsigned int width, height;
    std::vector<std::vector<mask_type>> mask;
    bool mask_is_int;
    Mask(unsigned int x, unsigned int y, std::initializer_list<int32_t> m)
        :width(x), height(y)
    {
        type = AbstractionType::Mask;
        set_mask(m);
    }
    Mask(unsigned int x, unsigned int y, std::initializer_list<float> m)
        :width(x), height(y)
    {
        type = AbstractionType::Mask;
        set_mask(m);
    }
    void set_mask(std::initializer_list<int32_t> m)
    {
        if (m.size() != height * width)
            throw std::runtime_error("Mask::set_mask: m needs to have x * y elements");

        mask_is_int = true;
        mask.clear();
        auto m_it = m.begin();
        for(unsigned int y = 0; y < height; y++)
        {
            mask.push_back(std::vector<mask_type>());
            for(unsigned x = 0; x < width; x++)
            {
                mask[y].push_back(*m_it);
                m_it++;
            }
        }
    }

    void set_mask(std::initializer_list<float> m)
    {
        if (m.size() != height * width)
            throw std::runtime_error("Mask::set_mask: m needs to have x * y elements");

        mask_is_int = false;
        mask.clear();
        auto m_it = m.begin();
        for(unsigned int y = 0; y < height; y++)
        {
            mask.push_back(std::vector<mask_type>());
            for(unsigned x = 0; x < width; x++)
            {
                mask[y].push_back(*m_it);
                m_it++;
            }
        }
    }

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

class LocalOperation: public AbstractionNode
{
public:
    std::vector<std::tuple<HipaVX::Image*, std::shared_ptr<ast4vx::WindowDescriptor>>> input_descriptor;
    std::vector<std::shared_ptr<ast4vx::WindowOperation>> operations;
    std::vector<std::vector<HipaVX::Image *>> operation_output_images;
    std::map<std::shared_ptr<ast4vx::WindowOperation>, std::vector<std::shared_ptr<DomVX::Mask>>> mask_bindings;
    std::map<std::shared_ptr<ast4vx::WindowDescriptor>, std::shared_ptr<DomVX::Domain>> domain_bindings;

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

    void set_masks(std::initializer_list<std::tuple<std::shared_ptr<ast4vx::WindowOperation>, std::initializer_list<std::shared_ptr<DomVX::Mask>>>> desc_mask_bindings)
    {
        mask_bindings.clear();
        for (auto desc: desc_mask_bindings)
            mask_bindings[std::get<0>(desc)] = std::get<1>(desc);
    }

    void set_masks(std::initializer_list<std::tuple<std::shared_ptr<ast4vx::WindowOperation>, std::shared_ptr<DomVX::Mask>>> desc_mask_bindings)
    {
        mask_bindings.clear();
        for (auto desc: desc_mask_bindings)
        {
            auto& masks = mask_bindings[std::get<0>(desc)];
            masks.clear();
            masks.emplace_back(std::get<1>(desc));
        }
    }

    void add_operation(std::shared_ptr<ast4vx::WindowOperation> op, std::initializer_list<HipaVX::Image*> out = {})
    {
        operations.emplace_back(op);
        operation_output_images.emplace_back(out);
    }

    void set_domains(std::initializer_list<std::tuple<std::shared_ptr<ast4vx::WindowDescriptor>, std::shared_ptr<DomVX::Domain>>> desc_domain_binding)
    {
        domain_bindings.clear();
        for (auto desc: desc_domain_binding)
            domain_bindings[std::get<0>(desc)] = std::get<1>(desc);
    }
};

}


template <class ReturnType, class ParameterType>
class AbstractionsVisitor
{
public:
    virtual ReturnType visit(std::shared_ptr<DomVX::AbstractionNode> n, ParameterType p) = 0;
};

