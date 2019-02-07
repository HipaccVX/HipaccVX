#include "../../VX/vx.h"
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/abstractions.hpp"
//#include "../include/ast_gen.hpp"

// A simple point operator
// TODO: generate functions should check whether the pointers are null or not

int main(int argc, const char *argv[]) {


    auto acc_i = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
    auto acc_o = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);

    // Map function, and changing its IOs
    DomVX::Map map, map2;
    map.function << assign(map.dout, sqrt(map.din));
    map2.function << assign(map2.dout, exp(map2.din));
    std::cout << "map2 function:" << generate(&map2.function);
    std::cout << "map function:" << generate(&map.function);
    DomVX::change_input(map, acc_i);
    DomVX::change_output(map, acc_o);
    std::cout << "map again: " << generate(&map.function) << std::endl;

    DomVX::Reduce red;
    red.function << assign(red.dout, exp(red.dleft) + exp(red.dright));
    std::cout << "red function:" << generate(&red.function);

    DomVX::Win2D<int> win(3, 3, {-1,  0,  1, -2,  0,  2, -1,  0,  1});
    win.dump_coeffs();
    win.dump_windids();
    std::cout << std::endl;

    //TODO: consider win.reduce().forall();
    win.forall(map).reduce(red);
    std::cout << "Mask function:" << std::endl
              << generate(&win.function) << std::endl;


    // Data pattern starts with a data, or window
    // Image scan is not considered

    return 0;
}

// Node{
//  std::vector<std::shared_ptr<Node>> subnodes;
// }
