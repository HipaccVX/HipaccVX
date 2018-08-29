
#include "../vx.h"
#include <string>

vx_node vxFWriteImageNode(vx_graph graph, vx_image image, std::string file);
vx_image vxCreateImageFromFile(vx_context context, vx_uint32 width, vx_uint32 height, vx_df_image color, std::string filename);

