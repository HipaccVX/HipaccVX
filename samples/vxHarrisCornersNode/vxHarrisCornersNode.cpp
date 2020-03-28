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

        int32_t two = 2;
        float strength_f = 0.00005f;
        vx_scalar strength = vxCreateScalar(context, VX_TYPE_FLOAT32, (void*)&strength_f);
        float min_distance_f = 5.0f;
        vx_scalar min_distance = vxCreateScalar(context, VX_TYPE_FLOAT32, (void*)&min_distance_f);
        float sensitivity_f = 0.04f;
        vx_scalar sensitivity = vxCreateScalar(context, VX_TYPE_FLOAT32, (void*)&sensitivity_f);
        vx_array corners = vxCreateArray(context, VX_TYPE_KEYPOINT, 100);
        vx_scalar corners_num = vxCreateScalar(context, VX_TYPE_INT32, (void*)&two);

        vx_image img[] = {
          vxCreateImageFromFile(context, width, height, VX_DF_IMAGE_U8, "200x300_bw_1.png"),
        };

        if (graph) {
            vx_node nodes[] = {
                vxHarrisCornersNode(graph, img[0], strength,
                              min_distance, sensitivity, 3, 3,
                              corners, corners_num),
            };
        }

        status = vxVerifyGraph(graph);

        if (status == VX_SUCCESS) {
            status = vxProcessGraph(graph);
        } else {
            printf("VERIFICATION ERROR: %d\n", status);
        }

	    for (int i = 0; i < 1; i++) vxReleaseImage(&img[i]);
    }
    vxReleaseContext(&context);

    return status;
}
