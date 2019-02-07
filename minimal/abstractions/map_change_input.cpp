#include "../../VX/vx.h"
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/abstractions.hpp"
#include "../include/ast_gen.hpp"

// A simple point operator
// TODO: generate functions should check whether the pointers are null or not

void change_output(function_ast::Statements &function, std::shared_ptr<function_ast::Node> n) {
    // I assume that there is only one output and that is the leftmost leaf
    auto it = function.statements.front()->subnodes[0];
    while(it->subnodes[0]->subnodes.size() > 0) {
        it = it->subnodes[0];
    }
    //std::cout << it->generate_source() << std::endl; 
    it->subnodes[0] = n;
}

void change_input(function_ast::Statements &function, std::shared_ptr<function_ast::Node> n) {
    // I assume that there is only one output and that is the leftmost leaf
    auto it = function.statements.back()->subnodes.back();
    while(it->subnodes.back()->subnodes.size() > 0) {
        it = it->subnodes.back();
    }
    //std::cout << it->generate_source() << std::endl; 
    it->subnodes.back() = n;
}

int main(int argc, const char *argv[]) {

    DomVX::Map map;

    auto acc_i = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
    auto acc_o = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
	auto pix_i = std::dynamic_pointer_cast<function_ast::Node>((std::make_shared<function_ast::Image>(acc_i)));
	auto pix_o = std::dynamic_pointer_cast<function_ast::Node>((std::make_shared<function_ast::Image>(acc_o)));

    // Map function, and changing its IOs
    map.function << assign(map.dout, sqrt(map.din));

    std::cout << "map function:" << std::endl;
    std::cout << "statement size " << map.function.statements.size() << std::endl;
    std::cout << "statement: " << map.function.statements[0]->generate_source() << std::endl;
    std::cout << "Subnode size: " << map.function.statements[0]->subnodes.size() << std::endl;
    std::cout << "(out) Subnodes - 0: " << map.function.statements[0]->subnodes[0]->generate_source() << std::endl;
    std::cout << "      Subnodes - 1: " << map.function.statements[0]->subnodes[1]->generate_source() << std::endl;
    std::cout << "(in)  Subnodes - 1: " << map.function.statements[0]->subnodes[1]->subnodes[0]->generate_source() << std::endl;
    std::cout << "generate: " << generate(&map.function);
    std::cout << std::endl << std::endl;

    std::cout << "test: " << map.function.statements[0]->subnodes[0]->subnodes[0]->subnodes.size() << std::endl;

    std::cout << "change generate input:" << std::endl;
    //map.function.statements[0]->subnodes[0] = std::dynamic_pointer_cast<function_ast::Node>(pix_o);
    //map.function.statements[0]->subnodes[1]->subnodes[0] = std::dynamic_pointer_cast<function_ast::Node>(pix_i);
    change_output(map.function, pix_o);
    change_input(map.function, pix_i);
    std::cout << "generate: " << generate(&map.function) << std::endl;
    
    return 0;
}

// Node{
//  std::vector<std::shared_ptr<Node>> subnodes;
// }
