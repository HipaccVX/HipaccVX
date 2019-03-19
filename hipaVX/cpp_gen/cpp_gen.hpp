#pragma once

#include "../ast.hpp"
#include "../abstractions.hpp"
#include "../gen_template.hpp"
#include "../kernels/domVX_kernels.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <map>



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



class CPPVisitor: public ASTVisitor<std::string, int>, public AbstractionsVisitor<std::string, int>
{
    std::vector<std::string> *pixelaccessor_mapping = nullptr;
    std::vector<std::string> *windowdescriptor_mapping = nullptr;
    std::vector<std::string> *maskaccessor_mapping = nullptr;

    std::string accumulator_string = "";

    std::string current_output_pixel_index = "";
    std::string current_output_y = "";
    std::string current_output_x = "";

    std::map<std::shared_ptr<ast4vx::WindowDescriptor>, std::string> desc_to_name;

public:
    virtual std::string visit(std::shared_ptr<ast4vx::Node> n, int i = 0) override;

    virtual std::string visit(std::shared_ptr<DomVX::AbstractionNode> n, int i = 0) override;

    std::string setup_outer_loop(std::shared_ptr<DomVX::Map> m);
    std::string setup_outer_loop(std::shared_ptr<DomVX::LocalOperation> m, const std::vector<HipaVX::Image *> &out);
};

