/*#include "../dsl/ast.hpp"
#include "../gen_template.hpp"
#include "../kernels/domVX_kernels.hpp"

#include "config_reader.hpp"

#include <iostream>
#include <string>
#include <vector>


#pragma once

using std::string;

static string hipaVX_folder = "hipaVX/hipacc_gen";

static std::map<vx_df_image, string> VX_DF_IMAGE_to_hipacc = {
    {VX_DF_IMAGE_U8, "uchar"},
    {VX_DF_IMAGE_S16, "short"},
    {VX_DF_IMAGE_S32, "int"},
    {VX_DF_IMAGE_U32, "uint"},
    {VX_DF_IMAGE_RGBX, "uchar4"},
    {VX_TYPE_FLOAT32, "float"}, //Not really a vx_df_image type
    {VX_TYPE_INT8, "uchar"}, //Not really a vx_df_image type
    {VX_TYPE_INT32, "int"} //Not really a vx_df_image type
};

string generate_image_name(HipaVX::Image *image);

void process_graph(HipaVX::Graph *graph);

class HipaccVisitor: public ASTVisitor<std::string, int>
{
public:
    virtual std::string visit(std::shared_ptr<ast4vx::Node> n, int i) override;
};
*/
