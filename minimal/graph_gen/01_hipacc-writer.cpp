// export CPLUS_INCLUDE_PATH=../../
// g_dag++ -Wall -Werror 07*.cpp ../../hipaVX/domVX_types.cpp

#include <iostream>
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/abstractions.hpp"
#include "../../hipaVX/graph.hpp"
#include "../../hipaVX/ast.hpp"
#include "../../hipaVX/graph_gen/hipacc_graph_gen.hpp"


using graphVX::AppGraphT;
using graphVX::OptGraphT;
using graphVX::dag;
using HipaVX::Image;
using HipaVX::Node;
using HipaVX::Scalar;


int main() {
//  hipacc_writer gen;
//  std::cout << gen.dump_code();

  // image
  HipaVX::Image im0(1024, 1024, VX_DF_IMAGE_U8);
  HipaVX::Image im1(1024, 1024);
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

//   // local
//   auto l_to_p = std::make_shared<ast4vx::LocalToPixel>(1, 1);
//   auto win_1 = l_to_p->window(0);
//   l_to_p << assign(l_to_p->d_out(0), win_1->pixel_at(0, 2) + win_1->pixel_at(0, 4));
//
//   auto local = ast4vx::WindowOperation();
//   local.set_window_inputs({std::make_shared<ast4vx::WindowDescriptor>(dom)});
//   local.to_pixel(l_to_p);
//
//   auto local_op = DomVX::LocalOperation();
//   local_op.set_input_window_desc({{&im0, std::make_shared<ast4vx::WindowDescriptor>(dom)}});
//   local_op.add_operation(std::make_shared<ast4vx::WindowOperation>(local), {&im1});

  // generator functions
  std::stringstream ss;

//  // accessor
//  //HipaccAccessor acc;
//  HipaccAccessor acc(&im0);
//  HipaccIterationSpace is(&im1);

//  // kernels: class declarations
//  gen.def(ss, &map, DefType::Kdecl, {&acc, &acc, &acc}, {&is});
//  gen.def(ss, &local_op, DefType::Kdecl, {&acc}, {&is}, {&mask}, {&dom});
//
//  // host declarations
//  gen.def(ss, &im0);
//  gen.def(ss, &acc);
//  gen.def(ss, &is);
//  gen.def(ss, &dom);
//  gen.def(ss, &mask);
//  ss << "\n\n";
//
//  // kernels: host declarations
//  gen.def(ss, &map, DefType::Hdecl, {&acc}, {&is});
//  gen.def(ss, &local_op, DefType::Hdecl, {&acc}, {&is});
//  gen.def(ss, &local_op, DefType::Hdecl, {&acc}, {&is}, {&mask});
//  gen.def(ss, &local_op, DefType::Hdecl, {&acc}, {&is}, {&mask}, {&dom});
//  ss << "\n\n";


  std::cout << ss.str();
  // Or
  // gen.def(&im);
  // std::cout << gen.ss_im.str();
  return 0;
}
