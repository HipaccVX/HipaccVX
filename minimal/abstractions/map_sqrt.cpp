#include "../../VX/vx.h"
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/abstractions.hpp"
#include "../../hipaVX/cpp_gen/cpp_gen.hpp"
#include <string>

// A simple point operator
// TODO: generate functions should check whether the pointers are null or not

int main(int argc, const char *argv[]) {

    CPPVisitor v;
    auto map = std::make_shared<DomVX::MapTest>();

    //auto acc_i = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
    //auto acc_o = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
	//auto pix_i = std::make_shared<function_ast::Image>(acc_i);
	//auto pix_o = std::make_shared<function_ast::Image>(acc_o);
    //
	//map.function << assign(target_pixel(pix_o), sqrt(current_pixel(pix_i)));

    // auto acc_i = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
    // auto acc_o = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
	// auto pix_i = std::make_shared<function_ast::CurrentPixelvalue>(std::make_shared<function_ast::Image>(acc_i));
	// auto pix_o = std::make_shared<function_ast::TargetPixel>      (std::make_shared<function_ast::Image>(acc_o));
    // 
	// map.function << assign(pix_o, sqrt(pix_i));

    //auto acc_i = std::make_shared<function_ast::Image>(new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8));
    //auto acc_o = std::make_shared<function_ast::Image>(new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8));
    //map.din = current_pixel(acc_i);
    //map.dout = target_pixel(acc_i);
	//map.function << assign(map.dout, sqrt(map.din));
	
    auto image_i = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
    auto image_o = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);

    auto ast_fun = std::make_shared<function_ast::Statements>(2);
    ast_fun << assign(ast_fun->pixel_accessor(0), sqrt(ast_fun->pixel_accessor(1)));

    map->set_statements(ast_fun);
    map->register_image({image_o, image_i});

    std::cout << v.visit(map);

    return 0;
}
