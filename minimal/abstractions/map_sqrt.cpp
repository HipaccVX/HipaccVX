#include "../../VX/vx.h"
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/abstractions.hpp"
#include "../include/ast_gen.hpp"

// A simple point operator
// TODO: generate functions should check whether the pointers are null or not

int main(int argc, const char *argv[]) {

    DomVX::Map map;

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
	
    auto acc_i = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
    auto acc_o = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
    map.function << assign(map.dout, sqrt(map.din));
    map.in_img = acc_i;
    map.out_img = acc_o;

    std::cout << generate(&map.function);

    return 0;
}

// Node{
//  std::vector<std::shared_ptr<Node>> subnodes;
// }
