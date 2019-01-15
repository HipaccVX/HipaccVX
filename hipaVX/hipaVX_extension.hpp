
#include "../VX/vx.h"
#include <string>

vx_node vxFWriteImageNode(vx_graph graph, vx_image image, std::string file);
vx_image vxCreateImageFromFile(vx_context context, vx_uint32 width, vx_uint32 height, vx_df_image color, std::string filename);
vx_node vxHipaccNode(vx_graph graph, std::string filename, vx_reference *parameters, vx_size count, vx_image out);
void vxDrawDotGraph(vx_graph graph, std::string filename, vx_uint32 node_depth = 0);

vx_node vxAnotherBilateralFilterNode(vx_graph graph, vx_image image, vx_int32 sigma_r, vx_image out);

