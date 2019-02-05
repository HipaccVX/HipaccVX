//#include "node_gen.hpp"
//#include "hipacc_gen.hpp"
#include "../gen_template.hpp"
#include "../ast.hpp"

#include <iostream>
#include <cstdlib>

#include <fstream>
#include <string>
#include <sstream>
#include <map>

#pragma once

// TODO: have nothing from HipaVX namespace

namespace function_ast
{
    std::string to_string(Datatype d);
    std::string generate_image_name(Image *i);

    // TODO: move to ast_gen.hpp
    std::string generate(Variable *s);
    std::string generate(VariableDefinition *s);
    std::string generate(SimpleBinaryNode *s);
    std::string generate(SimpleUnaryFunctionNode *s);
    std::string generate(If *s);
    std::string generate(Else *s);
    std::string generate(Vect4 *s);
    std::string generate(Extract4 *s);
    std::string generate(Assignment *s);

    // ??
    std::string generate(Image *s); // TODO: get rid of image member
    std::string generate(TargetPixel *s);
    std::string generate(CurrentPixelvalue *s);
    std::string generate(Statements *s);

    // ???
    std::string generate(ForEveryPixel *s);
    std::string generate(IterateAroundPixel *s);
    std::string generate(ReduceAroundPixel *s);
    std::string generate(PixelvalueAtCurrentStencilPos *s);
    std::string generate(StencilvalueAtCurrentStencilPos *s);
    // Not even implemented
    std::string generate(Stencil *s);
    std::string generate(ReductionOutput *s);

    // TODO: Better name?
    template<typename T>
    std::string generate(Constant<T> *s)
    {
        return std::to_string(s->value);
    }

    // Template Node method definitions
    template<typename T>
    std::string Constant<T>::generate_source()
    {
        return generate(this);
    }
}
