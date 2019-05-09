#pragma once

#include "../abstractions.hpp"
#include "../ast.hpp"
#include "../gen_template.hpp"
#include "../kernels/domVX_kernels.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>

using std::string;

static string hipaVX_folder = "hipaVX/cpp_gen";

static std::map<vx_df_image, string> VX_DF_IMAGE_to_cpp = {
    {VX_DF_IMAGE_U8, "unsigned char"},
    {VX_DF_IMAGE_S16, "short"},
    {VX_DF_IMAGE_S32, "int"},
    {VX_DF_IMAGE_U32, "unsigned int"},
    {VX_DF_IMAGE_RGBX, "uchar4"},
    {VX_TYPE_FLOAT32, "float"},       // Not really a vx_df_image type
    {VX_TYPE_INT8, "unsigned char"},  // Not really a vx_df_image type
    {VX_TYPE_INT32, "int"}            // Not really a vx_df_image type
};

string generate_image_name(DomVX::Image *image);

void process_graph(DomVX::Graph *graph);

class CPPVisitor : public ASTVisitor<std::string, int>,
                   public AbstractionsVisitor<std::string, int> {
  std::vector<std::string> *variableaccessor_mapping = nullptr;
  std::vector<std::string> *pixelaccessor_mapping = nullptr;
  std::vector<std::string> *windowdescriptor_mapping = nullptr;
  std::vector<std::string> *maskaccessor_mapping = nullptr;

  std::string accumulator_string = "";

  std::string current_output_pixel_index = "";
  std::string current_output_y = "";
  std::string current_output_x = "";

  std::map<std::shared_ptr<ast4vx::WindowDescriptor>, std::string> desc_to_name;
  std::map<std::shared_ptr<ast4vx::WindowDescriptor>,
           std::shared_ptr<DomVX::Domain>>
      desc_to_dom;

 public:
  virtual std::string visit(std::shared_ptr<ast4vx::Node> n,
                            int i = 0) override;

  virtual std::string visit(std::shared_ptr<DomVX::AbstractionNode> n,
                            int i = 0) override;

  std::string setup_outer_loop(std::shared_ptr<DomVX::Map> m);
  std::string setup_outer_loop(std::shared_ptr<DomVX::LocalOperator> m,
                               const std::vector<DomVX::Image *> &out);
  std::string setup_outer_loop(std::shared_ptr<DomVX::GlobalOperator> m,
                               const std::vector<DomVX::Image *> &in);
};
