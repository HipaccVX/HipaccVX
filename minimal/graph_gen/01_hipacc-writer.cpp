// export CPLUS_INCLUDE_PATH=../../
// g_dag++ -Wall -Werror 07*.cpp ../../hipaVX/domVX_types.cpp

#include <iostream>
#include "../../hipaVX/dsl/types.hpp"
//#include "../../hipaVX/dsl/ast.hpp"
#include "../../hipaVX/dsl/abstractions.hpp"
#include "../../hipaVX/graph/graph.hpp"
#include "../../hipaVX/gen/hipacc_graph.hpp"


using graphVX::AppGraphT;
using graphVX::OptGraphT;
using graphVX::dag;
using DomVX::Image;
using DomVX::Node;
using DomVX::Scalar;


int main() {
  hipacc_writer gen;
  std::cout << gen.initial_includes();

  // image
  DomVX::Image im0(1024, 1024, VX_DF_IMAGE_U8);
  DomVX::Image im1(1024, 1024);
  im1.set_dtype(VX_DF_IMAGE_U8);

 auto dom = create_dom(3, 5, {0, 0, 1,
                              0, 1, 0,
                              1, 0, 0,
                              0, 1, 0,
                              0, 0, 1});
 auto mask = create_mask(3, 5,
       {-1, -1,  1,
        -1,  0,  1,
        -1,  0,  1,
        -1,  0,  1,
        -1,  1,  1});

  // map
  auto ast_fun = std::make_shared<ast4vx::Statements>(1, 1);
  ast_fun << assign(ast_fun->d_out(0), sqrt(ast_fun->d_in(0)));

  auto map = create_point_op();
  map->set_statements(ast_fun);
  map->register_images({&im0}, {&im1});

  // local
  auto l_to_p = create_l2p(1, 1);
  auto win_1 = l_to_p->w_in(0);
  l_to_p << assign(l_to_p->d_out(0), win_1->pixel_at(0, 2) + win_1->pixel_at(0, 4) * constant(3.14f));

  auto window_in = create_window_desc(3, 5);

  auto local_to_pixel = create_window_op();
  local_to_pixel->set_window_inputs({window_in});
  local_to_pixel->to_pixel(l_to_p);

  auto local_op = create_local_op();
  local_op->set_input_window_desc({{&im0, window_in}});
  local_op->add_operation(local_to_pixel, {&im1});

  // generator functions
  std::stringstream ss;

  // accessor
  //HipaccAccessor acc;
  HipaccAccessor acc(&im0, true);
  HipaccIterationSpace is(&im1, false);

  // kernels: class declarations
  gen.def(ss, map.get(), DefType::Kdecl, {&acc, &acc, &acc}, {&is});
  gen.def(ss, local_op.get(), DefType::Kdecl, {&acc}, {&is}, {mask.get()}, {dom.get()});

//  // host declarations
  gen.def(ss, &im0);
  gen.def(ss, &acc);
  gen.def(ss, &is);
  gen.def(ss, dom.get());
  gen.def(ss, mask.get());
  ss << "\n\n";

  // kernels: host declarations
  gen.def(ss, map.get(), DefType::Hdecl, {&acc}, {&is});
  gen.def(ss, local_op.get(), DefType::Hdecl, {&acc}, {&is});
  gen.def(ss, local_op.get(), DefType::Hdecl, {&acc}, {&is}, {mask.get()});
  gen.def(ss, local_op.get(), DefType::Hdecl, {&acc}, {&is}, {mask.get()}, {dom.get()});
  ss << "\n\n";

  std::cout << ss.str();
  // Or
  // gen.def(&im);
  // std::cout << gen.ss_im.str();
  return 0;
}
