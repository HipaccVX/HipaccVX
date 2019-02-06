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
#include "ast.hpp"

#pragma once

namespace DomVX
{

// TODO: This header should be independent from the vx_types
//        move Image definition to here, and rename to Acc2D

enum class AbstractionType
{
    None, 
    Map,
    LocalOp
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
    std::vector<std::shared_ptr<function_ast::Node>> subnodes;
    std::shared_ptr<function_ast::Node>& operator[](unsigned int index) { 
        return subnodes[index]; 
    }
    std::shared_ptr<function_ast::Node> operator[](unsigned int index) const { 
        return subnodes[index]; 
    }

    virtual ~AbstractionNode() = default;
    virtual std::string generate_source() = 0;
};

class Map: public AbstractionNode
{
public:
    Map()
    {
        type = AbstractionType::Map;
    }
    HipaVX::Image* in_img = new HipaVX::Image(1, 1, VX_DF_IMAGE_U8);
    HipaVX::Image* out_img = new HipaVX::Image(1, 1, VX_DF_IMAGE_U8);

    std::shared_ptr<function_ast::Node> din = std::make_shared<function_ast::CurrentPixelvalue>(std::make_shared<function_ast::Image>(in_img));
    std::shared_ptr<function_ast::Node> dout = std::make_shared<function_ast::CurrentPixelvalue>(std::make_shared<function_ast::Image>(out_img));
    function_ast::Statements function;
    virtual std::string generate_source() override {
        return "implement this";
    };
};

}


//std::string visitor_cpp_raster(){
//    std::string to_return ="";
//
//    to_return += ;
//
//   return to_return;
//}

//class LocalOp: public Node
//{
//public:
//    LocalOp()
//    {
//        type = NodeType::LocalOp;
//    }
//    std::string name;
//    std::vector<std::string> mask;
//    int dim[2]; //dim[0] = x, dim[1] = y
//    Datatype datatype;
//    virtual std::string generate_source() override;
//
//    template<typename T>
//    static std::vector<std::string> from_t(std::vector<T> v)
//    {
//        std::vector<std::string> to_return;
//        to_return.reserve(v.size());
//        for(auto e: v)
//            to_return.emplace_back(std::to_string(e));
//        return to_return;
//    }
//};
