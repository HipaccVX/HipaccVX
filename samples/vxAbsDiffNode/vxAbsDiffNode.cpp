#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "VX/vx.h"
#include "VX/vx_compatibility.h"
#include "hipaVX/domVX_extensions.hpp"

int main(int argc, char *argv[]) {
#ifdef HIPAVX_OUTPUT_FILENAME
    set_output_filename(HIPAVX_OUTPUT_FILENAME);
#endif
    vx_uint32 width = 200;
    vx_uint32 height = 300;

    vx_context context = vxCreateContext();

    vx_status status = VX_FAILURE;

    if (context) {
        vx_graph graph = vxCreateGraph(context);

        vx_image img[] = {
          vxCreateImageFromFile(context, width, height, VX_DF_IMAGE_U8, "200x300_bw_1.png"),
          vxCreateImageFromFile(context, width, height, VX_DF_IMAGE_U8, "200x300_bw_2.png"),
          vxCreateImage(context, width, height, VX_DF_IMAGE_U8)
        };

        if (graph) {
            vx_node nodes[] = {
                vxAbsDiffNode(graph, img[0], img[1], img[2]),
            };
        }

        status = vxVerifyGraph(graph);

        if (status == VX_SUCCESS) {
            vxWriteImageAfterGraphCompletion(graph, img[2], "vxAbsDiffNode.png");
            status = vxProcessGraph(graph);
        } else {
            printf("VERIFICATION ERROR: %d\n", status);
        }

	    for (int i = 0; i < 3; i++) vxReleaseImage(&img[i]);
    }
    vxReleaseContext(&context);

    return status;
}
