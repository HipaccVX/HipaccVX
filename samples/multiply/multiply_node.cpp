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
          vxCreateImageFromFile(context, width, height, VX_DF_IMAGE_U8, "akif-200x300_bw.png"),
          vxCreateImageFromFile(context, width, height, VX_DF_IMAGE_U8, "akif-200x300_bw.png"),
          vxCreateImage(context, width, height, VX_DF_IMAGE_U8)
        };
	    vx_float32 scale = 1;
	    vx_scalar scale_s = vxCreateScalar(context, VX_TYPE_FLOAT32, &scale);
	    vx_enum convt_p = static_cast<vx_enum>(VX_CONVERT_POLICY_SATURATE);
	    //vx_enum convt_p = static_cast<vx_enum>(VX_CONVERT_POLICY_WRAP);
	    vx_enum round_e = static_cast<vx_enum>(VX_ROUND_POLICY_TO_ZERO);

        if (graph) {
            vx_node nodes[] = {
			    vxMultiplyNode(graph, img[0], img[1], scale_s, convt_p, round_e, img[2])
            };
        }

        status = vxVerifyGraph(graph);

        if (status == VX_SUCCESS) {
            status = vxProcessGraph(graph);
            vxWriteImageAfterGraphCompletion(graph, img[2], "mult.png");
        } else {
            printf("VERIFICATION ERROR: %d\n", status);
        }

        vxReleaseGraph(&graph);
	    for (int i = 0; i < 2; i++) vxReleaseImage(&img[i]);
    }
    vxReleaseContext(&context);

    return status;
}
