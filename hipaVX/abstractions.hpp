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
    Map,
    Reduce,
    Local,
    LocalOp,
    ForEveryPixelTest,
    MapTest
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
public:
    Map()
    {
        type = AbstractionType::Map;
    }
    HipaVX::Image* in_img = new HipaVX::Image(1, 1, VX_DF_IMAGE_U8);
    HipaVX::Image* out_img = new HipaVX::Image(1, 1, VX_DF_IMAGE_U8);
    std::shared_ptr<function_ast::Statements> function = std::make_shared<function_ast::Statements>();

    // TODO: change the printer functions to determine the target and source pixels at the time of printing
    std::shared_ptr<function_ast::Node> din = std::make_shared<function_ast::CurrentPixelvalue>(std::make_shared<function_ast::Image>(in_img));
    std::shared_ptr<function_ast::Node> dout = std::make_shared<function_ast::CurrentPixelvalue>(std::make_shared<function_ast::Image>(out_img));
    virtual std::string generate_source() override {
        return "implement this";
    };
};

class MapTest: public AbstractionNode
{
    std::shared_ptr<function_ast::Statements> function;
public:
    MapTest()
    {
        type = AbstractionType::MapTest;
    }
    //images are only the input images; therefore index 1,2,...
    //0 is reserved for output image
    MapTest(std::shared_ptr<function_ast::Statements> s, std::initializer_list<HipaVX::Image*> images)
    {
        type = AbstractionType::MapTest;
        set_statements(s);
        register_input_image(images);
    }

    std::vector<HipaVX::Image *> pixel_mappings;

    void set_statements(std::shared_ptr<function_ast::Statements> s)
    {
        function = s;
        pixel_mappings.resize(s->pixel_accessors.size());
    }

    std::shared_ptr<function_ast::Statements> get_statements()
    {
        return function;
    }

    void register_image(HipaVX::Image* image, unsigned int index)
    {
        if (index >= pixel_mappings.size())
            throw std::runtime_error("void MapTest::register_image(HipaVX::Image* image, unsigned int index): Index out of bounds");

        pixel_mappings[index] = image;
    }
    void register_image(std::initializer_list<HipaVX::Image*> images)
    {
        unsigned int index = 0;
        for (auto image: images)
        {
            register_image(image, index);
            index++;
        }
    }
    void register_input_image(std::initializer_list<HipaVX::Image*> images)
    {
        unsigned int index = 1;
        for (auto image: images)
        {
            register_image(image, index);
            index++;
        }
    }
    void register_image(std::initializer_list<HipaVX::Image*> images, std::initializer_list<unsigned int> indexes)
    {
        auto it_index = indexes.begin();
        for (auto image: images)
        {
            if (it_index == indexes.end())
                throw std::runtime_error("void Map::register_image(std::initializer_list<HipaVX::Image*> images, std::initializer_list<int> indexes): indexes smaller than images");
            register_image(image, *it_index);
            it_index++;
        }
    }
private:
    class MapVisitor: public ASTVisitor<std::shared_ptr<function_ast::Node>, int>
    {
        std::vector<HipaVX::Image*>& pixel_mappings;
    public:
        MapVisitor(std::vector<HipaVX::Image*>& pixel_mappings)
            :pixel_mappings(pixel_mappings)
        {}
        virtual std::shared_ptr<function_ast::Node> visit(std::shared_ptr<function_ast::Node> visiting, int i) override
        {
            switch(visiting->type)
            {
            case function_ast::NodeType::None:
                throw std::runtime_error("MapVisitor visited None");
            case function_ast::NodeType::Add:
            case function_ast::NodeType::Sub:
            case function_ast::NodeType::Mul:
            case function_ast::NodeType::Div:
            case function_ast::NodeType::ShiftLeft:
            case function_ast::NodeType::ShiftRight:
            case function_ast::NodeType::Less:
            case function_ast::NodeType::LessEquals:
            case function_ast::NodeType::Equals:
            case function_ast::NodeType::GreaterEquals:
            case function_ast::NodeType::Greater:
            case function_ast::NodeType::Unequals:
            case function_ast::NodeType::And:
            case function_ast::NodeType::Or:
            case function_ast::NodeType::Xor:
            case function_ast::NodeType::BitwiseAnd:
            case function_ast::NodeType::BitwiseOr:
            case function_ast::NodeType::BitwiseXor:
            case function_ast::NodeType::BitwiseNot:
            case function_ast::NodeType::Assignment:
            {
                visiting->subnodes[0] = visit(visiting->subnodes[0], i);
                visiting->subnodes[1] = visit(visiting->subnodes[1], i);
                return visiting;
            }
            case function_ast::NodeType::Not:
            case function_ast::NodeType::Sqrt:
            case function_ast::NodeType::Exp:
            case function_ast::NodeType::Conversion:
            case function_ast::NodeType::Abs:
            case function_ast::NodeType::Atan2:
            case function_ast::NodeType::Extract4:
            {
                visiting->subnodes[0] = visit(visiting->subnodes[0], i);
                return visiting;
            }
            case function_ast::NodeType::Vect4:
            {
                visiting->subnodes[0] = visit(visiting->subnodes[0], i);
                visiting->subnodes[1] = visit(visiting->subnodes[1], i);
                visiting->subnodes[2] = visit(visiting->subnodes[2], i);
                visiting->subnodes[3] = visit(visiting->subnodes[3], i);
                return visiting;
            }
            case function_ast::NodeType::If:
            {
                auto s = std::dynamic_pointer_cast<function_ast::If>(visiting);
                s->condition = visit(s->condition, i);
                visit(s->body, i);
                return visiting;
            }
            case function_ast::NodeType::Else:
            {
                auto s = std::dynamic_pointer_cast<function_ast::Else>(visiting);
                visit(s->body, i);
                return visiting;
            }
            case function_ast::NodeType::Statements:
            {
                auto s = std::dynamic_pointer_cast<function_ast::Statements>(visiting);
                for(unsigned int index = 0; index < s->statements.size(); index++)
                {
                    s->statements[index] = visit(s->statements[index], i);
                }
                return visiting;
            }

            //This is the real "magic"
            case function_ast::NodeType::PixelAccessor:
            {
                auto s = std::dynamic_pointer_cast<function_ast::PixelAccessor>(visiting);
                auto image = std::make_shared<function_ast::Image>();
                image->image = pixel_mappings.at(s->num);
                auto pixel = std::make_shared<function_ast::CurrentPixelvalue>();
                pixel->subnodes[0] = image;
                return pixel;
            }

            case function_ast::NodeType::ForEveryPixel:
            case function_ast::NodeType::IterateAroundPixel:
            case function_ast::NodeType::ReduceAroundPixel:
            case function_ast::NodeType::TargetPixel:
                throw std::runtime_error("MapGenerator: Should not happen? / Not supported yet? / Gonna get deleted anyway?)");
            // Doesn't have any childs
            case function_ast::NodeType::CurrentPixelvalue:
            case function_ast::NodeType::Stencil:
            case function_ast::NodeType::ReductionOutput:
            case function_ast::NodeType::PixelvalueAtCurrentStencilPos:
            case function_ast::NodeType::StencilvalueAtCurrentStencilPos:
            case function_ast::NodeType::Pregenerated:
            case function_ast::NodeType::WindowAccessor:
            case function_ast::NodeType::Image:
            case function_ast::NodeType::Constant:
            case function_ast::NodeType::Variable:
            case function_ast::NodeType::VariableDefinition:
            {
                return visiting;
            }
            default:
                break;
            }
            throw std::runtime_error("MapVisitor: reached end of switch case");
        }
    };
public:

    virtual std::string generate_source() override
    {
        MapVisitor v(pixel_mappings);
        v.visit(function, 0);
        return "implement this";
    }
};


class ForEveryPixelTest: public AbstractionNode
{
public:
    std::vector<std::shared_ptr<AbstractionNode>> calls;
    HipaVX::Image *out;
    ForEveryPixelTest(HipaVX::Image* output)
    {
        type = AbstractionType::ForEveryPixelTest;
        out = output;
    }
    ForEveryPixelTest& map(std::shared_ptr<MapTest> mapping)
    {
        mapping->register_image(out, 0);
        calls.emplace_back(mapping);
        return *this;
    }

    std::string generate_source()
    {
        return "";
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
    function_ast::Statements function;

    // TODO: change the printer functions to determine the target and source pixels at the time of printing
    std::shared_ptr<function_ast::Node> dleft = std::make_shared<function_ast::CurrentPixelvalue>(std::make_shared<function_ast::Image>(left_img));
    std::shared_ptr<function_ast::Node> dright = std::make_shared<function_ast::CurrentPixelvalue>(std::make_shared<function_ast::Image>(right_img));
    std::shared_ptr<function_ast::Node> dout = std::make_shared<function_ast::CurrentPixelvalue>(std::make_shared<function_ast::Image>(out_img));
    virtual std::string generate_source() override {
        return "implement this";
    };
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


template <class ReturnType, class ParameterType>
class AbstractionsVisitor
{
public:
    virtual ReturnType visit(std::shared_ptr<DomVX::AbstractionNode> n, ParameterType p) = 0;
};



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
