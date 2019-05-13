#include "../../hipaVX/dsl/ast.hpp"
#include "../include/ast_gen.hpp"

int main(int argc, const char *argv[]) {

    function_ast::ForEveryPixel kernel1;
    function_ast::ForEveryPixel kernel2;

    function_ast::Image in1, in2, out;
    auto in_node1 = std::make_shared<function_ast::Image>(in1);
    auto in_node2 = std::make_shared<function_ast::Image>(in2);
    auto out_node = std::make_shared<function_ast::Image>(out);

    // vect4
	auto temp = std::make_shared<function_ast::Variable>("temp", function_ast::Datatype::INT32);
	kernel1.function << assign(temp, current_pixel(in_node1) + current_pixel(in_node2));
 	kernel2.function << assign(target_pixel(out_node), temp * constant<int>(-32));

    kernel2.function.statements.insert(std::begin(kernel2.function.statements), std::begin(kernel1.function.statements), std::end(kernel1.function.statements));

    // print
    std::cout << generate(&kernel1.function);

    return 0;
}
