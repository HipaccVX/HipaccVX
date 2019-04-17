#include "../../../VX/vx.h"
#include "../../../hipaVX/domVX_types.hpp"
#include "../../../hipaVX/abstractions.hpp"
#include "../../../hipaVX/cpp_gen/cpp_gen.hpp"
#include <string>

int main()
{
    auto ast_reduction = std::make_shared<ast4vx::Reduction>(ast4vx::Constant<int>(255));
    auto accum_greater_if = IF(greater(ast_reduction->left(), ast_reduction->right()));
    accum_greater_if->body << assign(ast_reduction->out(), ast_reduction->right());
    ast_reduction << accum_greater_if;


    //---------------------------- DomVX --------------------------------------

    auto image_i = new HipaVX::Image(1024, 512, VX_DF_IMAGE_U8);
    auto var_out = new HipaVX::Scalar(VX_TYPE_UINT8);

    // Create the local operation
    auto global_op = std::shared_ptr<DomVX::GlobalOperation>(new DomVX::GlobalOperation());

    global_op->register_input_images({image_i});
    global_op->set_reduction_function(ast_reduction, var_out);


    CPPVisitor v;
    std::cout << v.visit(global_op) << "\n";

    return 0;
}

