#pragma once

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
#include "ast.hpp"
#include "domVX_types.hpp"

namespace DomVX
{

// TODO: This header should be independent from the vx_types - sure?
//        move Image definition to here, and rename to Acc2D

using AbstractionNode = DomVX::Object;
using OperatorNode = DomVX::Node;


/**
 * @brief Represents an actual Domain for the virtual ast4vx::WindowDescriptor
 */
class Domain : public AbstractionNode
{
public:

    unsigned int width, height;
    /**
     * @brief domain[height][width] with values 0 and 1
     */
    std::vector<std::vector<unsigned char>> domain;

    /**
     * @brief Constructs a new Domain with initialized values
     * @param x width of the domain
     * @param y height of the domain
     * @param dom row major flat matrix of the domain values. Has to have x*y entries
     */
    Domain(unsigned int x, unsigned int y, std::vector<int> dom)
        :width(x), height(y)
    {
        set_domain(dom);
        set_name("dom");
    }

    /**
     * @brief Updates the domain values
     * @param dom row major flat matrix of the domain values. Has to have x*y entries
     */
    void set_domain(std::vector<int> dom)
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

/**
 * @brief Represents an actual Mask for the virtual ast4vx::MaskPixelToPixel statements
 */
class Mask : public AbstractionNode
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
    /**
     * @brief mask[height][width] with either integer or float values
     */
    std::vector<std::vector<mask_type>> mask;
    bool mask_is_int;

    /**
     * @brief Constructs a new Mask with initialized values
     * @param x width of the mask
     * @param y height of the mask
     * @param m row major flat matrix of the int32_t mask values. Has to have x*y entries
     */
    Mask(unsigned int x, unsigned int y, std::vector<int32_t> m)
        :width(x), height(y)
    {
        set_mask(m);
        set_name("mask");
    }
    /**
     * @brief Constructs a new Mask with initialized values
     * @param x width of the mask
     * @param y height of the mask
     * @param m row major flat matrix of the float mask values. Has to have x*y entries
     */
    Mask(unsigned int x, unsigned int y, std::vector<float> m)
        :width(x), height(y)
    {
        set_mask(m);
        set_name("mask");
    }

    /**
     * @brief Updates the mask values
     * @param m row major flat matrix of the int32_t mask values. Has to have x*y entries
     */
    void set_mask(std::vector<int32_t> m)
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

    /**
     * @brief Updates the mask values
     * @param m row major flat matrix of the float mask values. Has to have x*y entries
     */
    void set_mask(std::vector<float> m)
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

/**
 * @brief Calculates every output pixel from the input image pixels from the same pixel coordinates
 */
class Map: public OperatorNode
{
    std::shared_ptr<ast4vx::Statements> function;
public:
    /**
     * @brief Default constructor
     */
    Map()
    {
        operator_type = OperatorType::Map;
    }

    /**
     * @brief Constructor which sets the statement as the mapping function
     * @param s The PixelToPixel mapping function
     */
    Map(std::shared_ptr<ast4vx::Statements> s)
    {
        operator_type = OperatorType::Map;
        set_statements(s);
    }

    std::vector<DomVX::Image *> output_pixel_mappings;
    std::vector<DomVX::Image *> input_pixel_mappings;
    std::vector<DomVX::Scalar *> output_variable_mappings;
    std::vector<DomVX::Scalar *> input_variable_mappings;

    /**
     * @brief Sets the statement as the mapping function
     * @param s The PixelToPixel mapping function
     */
    void set_statements(std::shared_ptr<ast4vx::Statements> s)
    {
        function = s;
        output_pixel_mappings.resize(s->out_pixel_mappings.size());
        input_pixel_mappings.resize(s->in_pixel_mappings.size());
        output_variable_mappings.resize(s->out_variable_mappings.size());
        input_variable_mappings.resize(s->in_variable_mappings.size());
    }

    /**
     * @brief Returns the mapping function statement
     */
    std::shared_ptr<ast4vx::Statements> get_statements()
    {
        return function;
    }

    /**
     * @brief Binds the output and input variables of the statements to the actual HipaVX::Scalar
     * @param out std::vector of the output variable bindings
     * @param in std::vector of the input variable bindings
     * @throws std::runtime_error when the length of the out std::vector is not the same es the size of the output placeholders of the underlying statements set via Map::set_statements
     * @throws std::runtime_error when the length of the in std::vector is not the same es the size of the input placeholders of the underlying statements set via Map::set_statements
     */
    void register_variables(std::vector<DomVX::Scalar*> out, std::vector<DomVX::Scalar*> in)
    {
        register_output_variables(out);
        register_input_variables(in);
    }
    /**
     * @brief Binds the input variables of the statements to the actual HipaVX::Scalar
     * @param variables std::vector of the input variable bindings
     * @throws std::runtime_error when the length of the std::vector is not the same es the size of the input placeholders of the underlying statements set via Map::set_statements
     */
    void register_input_variables(std::vector<DomVX::Scalar*> variables)
    {
        if (variables.size() != input_variable_mappings.size())
            throw std::runtime_error("void Map::register_input_variables(): Vector has to be the same size as the input_accessors of the underlying statemens");

        input_variable_mappings.clear();
        for(auto image: variables)
            input_variable_mappings.emplace_back(image);
    }
    /**
     * @brief Binds the output variables of the statements to the actual HipaVX::Scalar
     * @param variables std::vector of the output variable bindings
     * @throws std::runtime_error when the length of the std::vector is not the same es the size of the output placeholders of the underlying statements set via Map::set_statements
     */
    void register_output_variables(std::vector<DomVX::Scalar*> variables)
    {
        if (variables.size() != output_variable_mappings.size())
            throw std::runtime_error("void Map::register_output_variables(): Vector has to be the same size as the output_accessors of the underlying statemens");

        output_variable_mappings.clear();
        for(auto image: variables)
            output_variable_mappings.emplace_back(image);
    }

    /**
     * @brief Binds the output and input ast4vx::PixelAccessors of the statements to the actual HipaVX::Image
     * @param out std::vector of the output image bindings
     * @param in std::vector of the input image bindings
     * @throws std::runtime_error when the length of the out std::vector is not the same es the size of the output placeholders of the underlying statements set via Map::set_statements
     * @throws std::runtime_error when the length of the in std::vector is not the same es the size of the input placeholders of the underlying statements set via Map::set_statements
     */
    void register_images(std::vector<DomVX::Image*> out, std::vector<DomVX::Image*> in)
    {
        register_output_images(out);
        register_input_images(in);
    }
    /**
     * @brief Binds the input iamges of the statements to the actual HipaVX::Image
     * @param images std::vector of the input image bindings
     * @throws std::runtime_error when the length of the std::vector is not the same es the size of the input placeholders of the underlying statements set via Map::set_statements
     */
    void register_input_images(std::vector<DomVX::Image*> images)
    {
        if (images.size() != input_pixel_mappings.size())
            throw std::runtime_error("void Map::register_input_images(): Vector list has to be the same size as the input_accessors of the underlying statemens");

        input_pixel_mappings.clear();
        for(auto image: images)
            input_pixel_mappings.emplace_back(image);
    }
    /**
     * @brief Binds the output iamges of the statements to the actual HipaVX::Image
     * @param images std::vector of the output image bindings
     * @throws std::runtime_error when the length of the std::vector is not the same es the size of the output placeholders of the underlying statements set via Map::set_statements
     */
    void register_output_images(std::vector<DomVX::Image*> images)
    {
        if (images.size() != output_pixel_mappings.size())
            throw std::runtime_error("void Map::register_output_images(): Vector list has to be the same size as the output_accessors of the underlying statemens");

        output_pixel_mappings.clear();
        for(auto image: images)
            output_pixel_mappings.emplace_back(image);
    }
};

/**
 * @brief Calculates an output pixel from Domains and Masks of input images. The domains and masks center is at the current output pixels coordinate.
 *
 * It supports multiple input and output images - input domains and masks - and multiple ast4vx::WindowOperation
 */
class LocalOperation: public OperatorNode
{
public:
    std::vector<std::tuple<DomVX::Image*, std::shared_ptr<ast4vx::WindowDescriptor>>> input_descriptor;
    std::vector<std::shared_ptr<ast4vx::WindowOperation>> operations;

    // TODO: Have one parameter type
    std::vector<std::vector<DomVX::Image *>> operation_output_images; //TODO: Maybe strore IS
    std::vector<std::vector<DomVX::Scalar *>> operation_variables;

    // TODO: Do we really need these bindings, makes the code description tedious
    std::map<std::shared_ptr<ast4vx::WindowOperation>, std::vector<std::shared_ptr<DomVX::Mask>>> mask_bindings;
    std::map<std::shared_ptr<ast4vx::WindowDescriptor>, std::shared_ptr<DomVX::Domain>> domain_bindings;

public:
    LocalOperation()
    {
        operator_type = OperatorType::LocalOperation;
    }

    /**
     * @brief Maps the ast4vx::WindowDescriptor to actual HipaVX::Image. This operation clears the previous values set by this operation.
     * @param in_descriptors std::vector of std::tuple with an HipaVX::Image pointer which should be mapped to the ast4vx::WindowDescriptor and the ast4vx::WindowDescriptor itself
     */
    void set_input_window_desc(std::vector<std::tuple<DomVX::Image*, std::shared_ptr<ast4vx::WindowDescriptor>>> in_descriptors)
    {
        input_descriptor.clear();
        for (auto desc: in_descriptors)
            input_descriptor.emplace_back(desc);
    }

    /**
     * @brief Maps the ast4vx::WindowDescriptor to actual DomVX::Domain. This operation clears the previous values set by this operation.
     * @param desc_domain_binding std::vector of std::tuple with an ast4vx::WindowDescriptor std::shared_ptr and a DomVX::Domain std::shared_ptr which should be used for this ast4vx::WindowDescriptor
     */
    void set_domains(std::vector<std::tuple<std::shared_ptr<ast4vx::WindowDescriptor>, std::shared_ptr<DomVX::Domain>>> desc_domain_binding)
    {
        domain_bindings.clear();
        for (auto desc: desc_domain_binding)
            domain_bindings[std::get<0>(desc)] = std::get<1>(desc);
    }

    /**
     * @brief Sets a list of input DomVX::Mask for the specified ast4vx::WindowOperation. This operation clears the previous values set by this operation.
     * @param desc_mask_bindings std::vector of std::tuple with an ast4vx::WindowOperation std::shared_ptr and another std::vector of DomVX::Mask std::shared_ptr which should be mapped to the ast4vx::WindowOperation
     */
    void set_masks(std::vector<std::tuple<std::shared_ptr<ast4vx::WindowOperation>, std::vector<std::shared_ptr<DomVX::Mask>>>> desc_mask_bindings)
    {
        mask_bindings.clear();
        for (auto desc: desc_mask_bindings)
            mask_bindings[std::get<0>(desc)] = std::get<1>(desc);
    }

    /**
     * @brief Sets a list of input DomVX::Mask for the specified ast4vx::WindowOperation. This operation clears the previous values set by this operation.
     * @param desc_mask_bindings std::vector of std::tuple with an ast4vx::WindowOperation std::shared_ptr and a DomVX::Mask std::shared_ptr which should be mapped to the ast4vx::WindowOperation
     */
    void set_masks(std::vector<std::tuple<std::shared_ptr<ast4vx::WindowOperation>, std::shared_ptr<DomVX::Mask>>> desc_mask_bindings)
    {
        mask_bindings.clear();
        for (auto desc: desc_mask_bindings)
        {
            auto& masks = mask_bindings[std::get<0>(desc)];
            masks.clear();
            masks.emplace_back(std::get<1>(desc));
        }
    }

    /**
     * @brief Adds an operation to this LocalOperation. The order of operations is derived from the order of calls to this method
     * @param op The ast4vx::WindowOperation which should get added
     * @param out Optional std::vector of HipaVX::Image pointers to be bound to the operations output ast4vx::PixelAccessor if it has some
     * @param var_bindings std::vector of HipaVX::Scalar pointers to be bound to the operations output and input ast4vx::VariableAccessor if it has some
     */
    void add_operation(std::shared_ptr<ast4vx::WindowOperation> op, std::vector<DomVX::Image*> out = {}, std::vector<DomVX::Scalar*> var_bindings = {})
    {
        operations.emplace_back(op);
        operation_output_images.emplace_back(out);
        operation_variables.emplace_back(var_bindings);
    }
};

/**
 * @brief Calculates an output value from an input image
 *
 * Currently only global reduction of an image is supported
 */
class GlobalOperation: public OperatorNode
{
public:
    std::shared_ptr<ast4vx::Reduction> reduction;
    DomVX::Scalar* reduction_out;
    std::vector<DomVX::Image *> input_pixel_mappings;

public:
    GlobalOperation()
    {
        operator_type = OperatorType::GlobalOperation;
    }

    /**
     * @brief Binds the input iamges of the statements to the actual HipaVX::Image
     * Currently only the first input image is used, since currently reduction only works on one Image
     * @param images std::vector of the input image bindings
     */
    void register_input_images(std::vector<DomVX::Image*> images)
    {
        input_pixel_mappings.clear();
        for(auto image: images)
            input_pixel_mappings.emplace_back(image);
    }

    /**
     * @brief Sets the reduction function for this GlobalOperation
     * @param red The ast4vx::Reduction statement which should be used for this computation
     * @param out The output HipaVX::Scalar pointer, which gets mapped to the output of the ast4vx::Reduction
     */
    void set_reduction_function(std::shared_ptr<ast4vx::Reduction> red, DomVX::Scalar* out)
    {
        reduction = red;
        reduction_out = out;
    }
};
}

// TODO: What is the motivation behind having this here?

/**
 * @brief Abstract Visitor class for DomVX::AbstractionNode
 */
template <class ReturnType, class ParameterType>
class AbstractionsVisitor
{
public:
    virtual ReturnType visit(std::shared_ptr<DomVX::AbstractionNode> n, ParameterType p) = 0;
};

