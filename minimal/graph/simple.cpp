//#include "kernels/domVX_kernels.hpp"
#include "../../VX/vx.h"
#include "domVX_types.hpp"
#include "abstractions.hpp"
//#include "cpp_gen/cpp_gen.hpp"
#include "hipacc_gen/hipacc_gen.hpp"
#include <string>

HipaVX::Node* MagnitudeNode(HipaVX::Graph* g, HipaVX::Image* x_im, HipaVX::Image* y_im, HipaVX::Image* mag_im) {
  auto node = new HipaVX::CompositeNode("vxMagnitude");
  node->inp(x_im);
  node->inp(y_im);
  node->outp(mag_im);

  auto *xx_n = new HipaVX::SquareNode();
  auto *yy_n = new HipaVX::SquareNode();
  auto *add_n = new HipaVX::SimplePointAdd();
  auto *sqrt_n = new HipaVX::SqrtNode();
  auto *sat_n = new HipaVX::SaturateNode();

  node->img_list.emplace_back(new HipaVX::Image(x_im->w, x_im->h, VX_DF_IMAGE_S32));
  node->img_list.emplace_back(new HipaVX::Image(y_im->w, y_im->h, VX_DF_IMAGE_S32));
  node->img_list.emplace_back(new HipaVX::Image(y_im->w, y_im->h, VX_DF_IMAGE_S32));
  node->img_list.emplace_back(new HipaVX::Image(y_im->w, y_im->h, VX_DF_IMAGE_S32));
  auto   xx_im = node->img_list[0].get();
  auto   yy_im = node->img_list[1].get();
  auto  add_im = node->img_list[2].get();
  auto sqrt_im = node->img_list[3].get();

  xx_n->in = x_im;
  yy_n->in = y_im;
  xx_n->out = xx_im;
  yy_n->out = yy_im;
  add_n->in_1 = xx_im;
  add_n->in_2 = yy_im;
  add_n->out = add_im;
  sqrt_n->in = add_im;
  sqrt_n->out = sqrt_im;
  sat_n->in = sqrt_im;
  sat_n->out = mag_im;

  node->subnodes.push_back(xx_n);
  node->subnodes.push_back(yy_n);
  node->subnodes.push_back(add_n);
  node->subnodes.push_back(sqrt_n);
  node->subnodes.push_back(sat_n);

	g->graph.emplace_back(node);
	g->built = true;

	return node;
};


int main() {
    HipaVX::Graph g;
    HipaVX::Image x_im, y_i, mag_im;

    auto node = MagnitudeNode(&g, &x_im, &y_i, &mag_im);

    HipaccVisitor gen;
//    CPPVisitor gen;
//    std::cout << gen.visit(node, 0);
    process_graph(&g);
}
