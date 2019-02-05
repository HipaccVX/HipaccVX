#include "../../hipaVX/ast.hpp"
#include "../include/ast_gen.hpp"

// A simple point operator

int main(int argc, const char *argv[]) {

    function_ast::ForEveryPixel kernel;

    function_ast::Image in, out;
	auto in_node = std::make_shared<function_ast::Image>(in);
	auto out_node = std::make_shared<function_ast::Image>(out);

    // if/else 
    auto threshold = constant<int>(255);
	auto if_less = IF(less(current_pixel(in_node), threshold));
         if_less->body << assign(target_pixel(out_node), threshold);
	auto else_less = ELSE();
	     else_less->body << assign(target_pixel(out_node), threshold);

    kernel.function << if_less;
    kernel.function << else_less;

    // print
    std::cout << generate(&kernel.function);
    
    return 0;
}
