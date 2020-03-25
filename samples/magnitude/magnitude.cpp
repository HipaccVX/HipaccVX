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
    vx_uint32 width = 640;
    vx_uint32 height = 480;

    vx_context context = vxCreateContext();

    vx_status status = VX_FAILURE;

    if (context) {
        vx_graph graph = vxCreateGraph(context);

        vx_image img[] = {
          vxCreateImageFromFile(context, width, height, VX_DF_IMAGE_S16, "akif-200x300_bw.png"),
          vxCreateImageFromFile(context, width, height, VX_DF_IMAGE_S16, "akif-200x300_bw.png"),
          vxCreateImage(context, width, height, VX_DF_IMAGE_S16),
        };

        if (graph) {
            vx_node nodes[] = {
                vxMagnitudeNode(graph, img[1], img[0], img[2]),
            };
        }

        status = vxVerifyGraph(graph);

        if (status == VX_SUCCESS) {
            status = vxProcessGraph(graph);
            vxWriteImageAfterGraphCompletion(graph, img[1], "grad_x.png");
            vxWriteImageAfterGraphCompletion(graph, img[2], "grad_y.png");
        } else {
            printf("VERIFICATION ERROR: %d\n", status);
        }

        vxReleaseGraph(&graph);
	    for (int i = 0; i < 2; i++) vxReleaseImage(&img[i]);
    }
    vxReleaseContext(&context);

    return status;
}