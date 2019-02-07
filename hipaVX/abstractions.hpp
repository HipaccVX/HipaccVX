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
#include "hipacc_gen/ast_gen.hpp"

#pragma once

namespace DomVX
{

// TODO: This header should be independent from the vx_types
//        move Image definition to here, and rename to Acc2D

enum class AbstractionType
{
    None, 
    Map,
    Reduce,
    Local,
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

template<class AbsNode>
void change_output(AbsNode &ptrn, HipaVX::Image *im) {
    // I assume that there is only one output and that is the leftmost leaf
    auto it = ptrn.function.statements.front()->subnodes[0];
    while(it->subnodes[0]->subnodes.size() > 0) {
        it = it->subnodes[0];
    }
    //std::cout << it->generate_source() << std::endl; 
    it->subnodes[0] = std::dynamic_pointer_cast<function_ast::Node>((std::make_shared<function_ast::Image>(im)));
}

template<class AbsNode>
void change_input(AbsNode &ptrn, HipaVX::Image *im) {
    // I assume that there is only one output and that is the leftmost leaf
    auto it = ptrn.function.statements.back()->subnodes.back();
    while(it->subnodes.back()->subnodes.size() > 0) {
        it = it->subnodes.back();
    }
    //std::cout << it->generate_source() << std::endl; 
    it->subnodes.back() = std::dynamic_pointer_cast<function_ast::Node>((std::make_shared<function_ast::Image>(im)));
}

// TODO: define window, pixel etc. and detach from Map, Win2D

class Map: public AbstractionNode
{
public:
    Map()
    {
        type = AbstractionType::Map;
    }
    HipaVX::Image* in_img = new HipaVX::Image(1, 1, VX_DF_IMAGE_U8);
    HipaVX::Image* out_img = new HipaVX::Image(1, 1, VX_DF_IMAGE_U8);
    function_ast::Statements function;

    // TODO: change the printer functions to determine the target and source pixels at the time of printing
    std::shared_ptr<function_ast::Node> din = std::make_shared<function_ast::CurrentPixelvalue>(std::make_shared<function_ast::Image>(in_img));
    std::shared_ptr<function_ast::Node> dout = std::make_shared<function_ast::CurrentPixelvalue>(std::make_shared<function_ast::Image>(out_img));
    virtual std::string generate_source() override {
        return "implement this";
    };
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
    function_ast::Statements function;

    // TODO: change the printer functions to determine the target and source pixels at the time of printing
    std::shared_ptr<function_ast::Node> dleft = std::make_shared<function_ast::CurrentPixelvalue>(std::make_shared<function_ast::Image>(left_img));
    std::shared_ptr<function_ast::Node> dright = std::make_shared<function_ast::CurrentPixelvalue>(std::make_shared<function_ast::Image>(right_img));
    std::shared_ptr<function_ast::Node> dout = std::make_shared<function_ast::CurrentPixelvalue>(std::make_shared<function_ast::Image>(out_img));
    virtual std::string generate_source() override {
        return "implement this";
    };
};


class Datum 
{
public:
    HipaVX::Image out_pix;
    std::vector<std::vector<HipaVX::Image>> buf;

    function_ast::Statements function;

    //template<typename WT>
    //Datum& reduce(Win2D<WT> win2d, DomVX::Reduce &red) {
    //    for(int i = 0; i < win2d.dom.size(); i++) {
    //        if (win2d.dom[i] == 0) continue;
    //        buf.emplace_back(std::vector<HipaVX::Image>());
    //        change_input(red, &win2d.buck.back()[0]);
    //        change_output(red, &win2d.buf.back()[0]);
    //        function.append(red.function);
    //    }
    //    return *this;
    //}
};


template<typename T>
class Win2D
{
public:
    // TODO: mask and domain doesn't have to be the same
    std::vector<HipaVX::Image> wnd; // input
    HipaVX::Image out_pix;
    std::vector<T> mask;
    std::vector<T> dom;
    int dimx, dimy;

    std::vector<std::vector<HipaVX::Image>> buf;


    Win2D(int dimx, int dimy) : dimx(dimx), dimy(dimy){
        mask.resize(dimx * dimy);
        wnd.resize(dimx * dimy);
        dom(mask);
    }
    Win2D(int dimx, int dimy, std::initializer_list<T> l) : dimx(dimx), dimy(dimy), mask(l), dom(mask) {
        wnd.resize(dimx * dimy);
        if ((dimx * dimy) != mask.size()) {
            std::cerr << "Error: Mask size does not match with the initialization list!" << std::endl;
            exit(1);
        } 
    }

    function_ast::Statements function;

    //auto iterate = std::make_shared<function_ast::IterateAroundPixel>();

    // TODO: Better way is to use graph structure we use in other places as well
    Win2D<T>& forall(DomVX::Map &map) {
        buf.emplace_back(std::vector<HipaVX::Image>());
        buf.back().resize(dom.size()); // TODO: sparse array
        // TODO: Add safety checks (NULL, Type, etc)
        for(int i = 0; i < dom.size(); i++) {
            if (dom[i] == 0) continue;
            // TODO: add clone to the ast nodes 
            change_input(map, &wnd[i]);
            change_output(map, &buf.back()[i]);
            function.append(map.function);
            //std::cout << generate(&map.function);
        }
        //return std::make_shared<Win2D<T>>(*this);
        return *this;
    }

    Win2D& reduce(DomVX::Reduce &red) {
        change_output(red, &out_pix);
        //change_input2(red, &out_pix);
        for(int i = 0; i < dom.size(); i++) {
            if (dom[i] == 0) continue;
            change_input(red, &buf.back()[i]);
            function.append(red.function);
        }
        return *this;
    }

    void dump_coeffs() {
        std::cout << "dims(x, y) : " << dimx << ", " << dimy << std::endl;
        std::cout << "size = " << mask.size() << std::endl;
        std::cout << "coeffs = ";
        for(auto it:mask ) std::cout << it << ", ";
        std::cout << std::endl;
    }
    void dump_windids() {
        std::cout << "window size = " << wnd.size() << std::endl;
        std::cout << "wnd ids = ";
        for(auto it:wnd ) std::cout << it.my_id << ", ";
        std::cout << std::endl;
    }
};


class Local: public AbstractionNode
{
public:
    Local()
    {
        type = AbstractionType::Local;
    }
    HipaVX::Image* in_img = new HipaVX::Image(1, 1, VX_DF_IMAGE_U8);
    HipaVX::Image* out_img = new HipaVX::Image(1, 1, VX_DF_IMAGE_U8);

    // TODO: change the printer functions to determine the target and source pixels at the time of printing
    std::shared_ptr<function_ast::Node> din = std::make_shared<function_ast::CurrentPixelvalue>(std::make_shared<function_ast::Image>(in_img));
    std::shared_ptr<function_ast::Node> dout = std::make_shared<function_ast::TargetPixel>(std::make_shared<function_ast::Image>(out_img));

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
