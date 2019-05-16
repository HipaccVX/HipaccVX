#pragma once

#include "../dsl/abstractions.hpp"
#include "../dsl/ast.hpp"
#include "../kernels/domVX_kernels.hpp"

#include <map>
#include <string>
#include <vector>

static std::map<vx_df_image, std::string> VX_DF_IMAGE_to_cpp = {
    {VX_DF_IMAGE_U8, "unsigned char"},
    {VX_DF_IMAGE_S16, "short"},
    {VX_DF_IMAGE_S32, "int"},
    {VX_DF_IMAGE_U32, "unsigned int"},
    {VX_DF_IMAGE_RGBX, "uchar4"},
    {VX_TYPE_FLOAT32, "float"},       // Not really a vx_df_image type
    {VX_TYPE_INT8, "unsigned char"},  // Not really a vx_df_image type
    {VX_TYPE_INT32, "int"}            // Not really a vx_df_image type
};

class CPPVisitor : public ASTVisitor<std::string, int> {
 public:
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
};
