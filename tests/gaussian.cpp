#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "VX/vx.h"
#include "VX/vx_compatibility.h"
#include "hipaVX/domVX_extensions.hpp"

int main(int argc, char *argv[]) {
    vx_uint32 width = 640;
    vx_uint32 height = 480;
    
    vx_context context = vxCreateContext();
    
    vx_status status = VX_FAILURE;

    if (context) {
        vx_graph graph = vxCreateGraph(context);
    
        vx_image img[] = {
          vxCreateImageFromFile(context, width, height, VX_DF_IMAGE_U8, "akif-200x300_bw.png"),
          vxCreateImage(context, width, height, VX_DF_IMAGE_U8)
        };
    
        if (graph) {
            vx_node nodes[] = {
                vxGaussian3x3Node(graph, img[1], img[0]),
            };
        }
          
        vx_status status = vxVerifyGraph(graph);
          
        if (status == VX_SUCCESS) {
            status = vxProcessGraph(graph);
            vxFWriteImageNode(graph, img[1], "res_gauss.png");
        } else {
            printf("VERIFICATION ERROR: %d\n", status);
        }

        vxDrawDotGraph(graph, "graph.dot", 0);
        vxReleaseGraph(&graph);
	    for (int i = 0; i < 2; i++) vxReleaseImage(&img[i]);
    }
    vxReleaseContext(&context);

    return status;
}
