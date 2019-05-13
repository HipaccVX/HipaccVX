#include <string>
#include "../../../VX/vx.h"
#include "../../../hipaVX/cpp_gen/cpp_gen.hpp"
#include "../../../hipaVX/dsl/types.hpp"
#include "../../../hipaVX/dsl/abstractions.hpp"
#include "../../../hipaVX/dsl/api.hpp"

int main() {
  //---------------------------- DomVX --------------------------------------

  auto image_i = create_image(1024, 512, VX_DF_IMAGE_U8);
  auto image_o = create_image(1024, 512, VX_DF_IMAGE_U8);

  auto mask = create_mask(3, 5, {-1, -1, 1, -1, 0,
                                  1, -1, 0, 1, -1,
                                  0, 1, -1, 1, 1});
  auto dom = create_domain(mask);

  auto win_in = get_window(image_i, dom);

  //---------------------------- Ast4VX --------------------------------------
  // TODO (CONFLIC1): We should actually have consistency for
  //        * create_pix2pix and create_win2pix

  // Created two sample pixel to pixel functions for later use
  auto ast_p2p_mult = create_p2p_mask(1, 1, 1); // (input, output, mask)
  ast_p2p_mult << assign(ast_p2p_mult->d_out(0),
                       ast_p2p_mult->d_in(0) * ast_p2p_mult->m_in(0));

  // TODO: Alternative ideas:
  // ast_p2p_mult->d_out(0) << ast_p2p_mult->d_in(0) * ast_p2p_mult->m_in(0);

  // Reduction AST expects an initial value for the accumulator
  auto ast_red_sum = create_reduction(ast4vx::Constant<float>(.0f));
  ast_red_sum << assign(ast_red_sum->out(),
                          ast_red_sum->left() + ast_red_sum->right());

  // There are handy functions for "forall" and "reduce", which reduces the
  // mundane calls
  auto win = forall(win_in, ast_p2p_mult);
  auto pix = reduce(win, ast_red_sum);
  // TODO: Alternative ideas:
  // win = forall(ast_p2p_mult)->in(win_in)
  // pix = reduce(ast_red_sum)->in(win)

  //---------------------------- DomVX --------------------------------------
  // TODO (CONFLICT2): mask belongs to forall operation (win) or to ast_p2p_mult
  //                   does setting mask for the ast via local op make sense?

  // 0. Create the local operation
  // 1. Bind the masks to their corresponding WindowOperations
  // 2. Add all operations which leads to this reduction operation (included),
  //    needs an output image
  auto local = create_local_op();       // TODO: What about:
  local->set_masks({{win, mask}});      //  ->set_mask(win(mask))
  local->add_operation(pix, {image_o}); //  ->add_operation(pix(image_o))

  // TODO: Alternative ideas:
  // local->op(win)->in(win_in)->mask(mask)
  // local->op(pix)->out(image_o)

  // TODO: Alternative ideas:
  // win = local->forall(ast_p2p_mult)->in(win_in)->mask(mask)
  // pix = local->reduce(ast_red_sum)->in(win)->out(image_o)
  //
  // also offer the seperate assignments
  // win_mask = forall(ast_p2p_mult)->mask(mask)
  // local->op(win_mask)->in(win_in)

  CPPVisitor v;
  std::cout << v.visit(local) << "\n";

  return 0;
}
