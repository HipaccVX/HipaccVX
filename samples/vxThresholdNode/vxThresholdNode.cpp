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

        vx_uint32 th = 170;
        vx_threshold thresh = vxCreateThresholdForImage(context, VX_THRESHOLD_TYPE_BINARY, VX_DF_IMAGE_U8, VX_DF_IMAGE_U8);
        vxSetThresholdAttribute(thresh, VX_THRESHOLD_THRESHOLD_VALUE, &th, sizeof(th));

        vx_graph graph = vxCreateGraph(context);

        vx_image img[] = {
          vxCreateImageFromFile(context, width, height, VX_DF_IMAGE_U8, "200x300_bw_1.png"),
          vxCreateImage(context, width, height, VX_DF_IMAGE_U8)
        };

        if (graph) {
            vx_node nodes[] = {
                vxThresholdNode(graph, img[0], thresh, img[1]),
            };
        }

        status = vxVerifyGraph(graph);

        if (status == VX_SUCCESS) {
            status = vxProcessGraph(graph);
            vxWriteImageAfterGraphCompletion(graph, img[1], "vxThresholdNode.png");
        } else {
            printf("VERIFICATION ERROR: %d\n", status);
        }

	    for (int i = 0; i < 2; i++) vxReleaseImage(&img[i]);
    }
    vxReleaseContext(&context);

    return status;
}
