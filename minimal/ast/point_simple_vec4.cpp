#include "../../hipaVX/ast.hpp"
#include "../include/ast_gen.hpp"

// A simple point operator

int main(int argc, const char *argv[]) {

    function_ast::ForEveryPixel kernel;

    function_ast::Image in1, in2, in3, in4, out;
	auto in_node1 = std::make_shared<function_ast::Image>(in1);
	auto in_node2 = std::make_shared<function_ast::Image>(in2);
	auto in_node3 = std::make_shared<function_ast::Image>(in3);
	auto in_node4 = std::make_shared<function_ast::Image>(in4);
	auto out_node = std::make_shared<function_ast::Image>(out);

    // vect4 
	kernel.function << assign(target_pixel(out_node), vect4(current_pixel(in_node1), current_pixel(in_node2), current_pixel(in_node3), current_pixel(in_node4), function_ast::Datatype::UCHAR4));

    // print
    std::cout << generate(&kernel.function);
    
    return 0;
}
