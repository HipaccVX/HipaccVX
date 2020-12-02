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

        vx_convolution laplacian_convolution = vxCreateConvolution(context, 5, 5);
        vx_int16 laplacian_coefficients[5][5] = {
                {  1, 1,   1, 1, 1},
                {  1, 1,   1, 1, 1},
                {  1, 1, -24, 1, 1},
                {  1, 1,   1, 1, 1},
                {  1, 1,   1, 1, 1},
            };
        vx_uint32 laplacian_scale = 1;
        vxCopyConvolutionCoefficients(laplacian_convolution, (vx_int16*)laplacian_coefficients, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);
        vxSetConvolutionAttribute(laplacian_convolution, VX_CONVOLUTION_SCALE, &laplacian_scale, sizeof(laplacian_scale));

        vx_image img[] = {
          vxCreateImageFromFile(context, width, height, VX_DF_IMAGE_U8, "200x300_bw_1.png"),
          vxCreateImage(context, width, height, VX_DF_IMAGE_U8)
        };

        if (graph) {
            vx_node nodes[] = {
                vxConvolveNode(graph, img[0], laplacian_convolution, img[1]),
            };
        }

        status = vxVerifyGraph(graph);

        if (status == VX_SUCCESS) {
            vxWriteImageAfterGraphCompletion(graph, img[1], "vxConvolveNode.png");
            status = vxProcessGraph(graph);
        } else {
            printf("VERIFICATION ERROR: %d\n", status);
        }

	    for (int i = 0; i < 2; i++) vxReleaseImage(&img[i]);
    }
    vxReleaseContext(&context);

    return status;
}
