#include "../../hipaVX/ast.hpp"
#include "../include/ast_gen.hpp"

// A simple point operator

int main(int argc, const char *argv[]) {

    function_ast::ForEveryPixel kernel;

    function_ast::Image in, out;
	auto in_node = std::make_shared<function_ast::Image>(in);
	auto out_node = std::make_shared<function_ast::Image>(out);

	kernel.function << assign(target_pixel(out_node), sqrt(current_pixel(in_node)));

    std::cout << generate(&kernel.function);
    
    return 0;
}
