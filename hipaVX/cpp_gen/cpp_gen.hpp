#include "../ast.hpp"
#include "../gen_template.hpp"
#include "../kernels/domVX_kernels.hpp"

#include "ast_gen.hpp"

#include <iostream>
#include <vector>
#include <string>


#pragma once

using std::string;

static string hipaVX_folder = "hipaVX/cpp_gen";

static std::map<vx_df_image, string> VX_DF_IMAGE_to_cpp = {
    {VX_DF_IMAGE_U8, "unsigned char"},
    {VX_DF_IMAGE_S16, "short"},
    {VX_DF_IMAGE_S32, "int"},
    {VX_DF_IMAGE_U32, "unsigned int"},
    {VX_DF_IMAGE_RGBX, "uchar4"},
    {VX_TYPE_FLOAT32, "float"}, //Not really a vx_df_image type
    {VX_TYPE_INT8, "unsigned char"}, //Not really a vx_df_image type
    {VX_TYPE_INT32, "int"} //Not really a vx_df_image type
};

string generate_image_name(HipaVX::Image *image);

void process_graph(HipaVX::Graph *graph);
