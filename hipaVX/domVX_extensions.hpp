#pragma once
#include <fstream>
#include <iostream>
#include <string>

#include "../VX/vx.h"
#include "vx_adaptor.hpp"

// TODO: vxCreateImageFromFile, vxFWriteImageNode, vxDrawDotGraph are not
// extensions.
//       consider moving these to a utils file

// TODO: use custom node APIs
// Hipacc interoperability for OpenVX
vx_node vxHipaccNode(vx_graph graph, std::string filename,
                     vx_reference *parameters, vx_size count, vx_image out);

// Read an image file to vx_image
vx_image vxCreateImageFromFile(vx_context context, vx_uint32 width,
                               vx_uint32 height, vx_df_image color,
                               std::string filename);

// Write a vx_image to a file
void vxWriteImageAfterGraphCompletion(vx_graph graph, vx_image image,
                                      std::string file);

// TODO: get rid of this one
// Write a vx_image to a file
vx_node vxAnotherBilateralFilterNode(vx_graph graph, vx_image image,
                                     vx_int32 sigma_r, vx_image out);

vx_kernel vxHipaccKernel(std::string filename);

vx_kernel vxCppKernel(std::string filename);

void set_output_filename(std::string filename);

vx_status vxAddParameterToHipaccKernel(vx_kernel kernel, vx_uint32 index,
                                       vx_enum dir,
                                       HipaccParameterType data_type,
                                       vx_enum state);
