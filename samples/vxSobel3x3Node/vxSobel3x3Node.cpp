#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "VX/vx.h"
#include "VX/vx_compatibility.h"
#include "hipaVX/domVX_extensions.hpp"

#define GEN_TEST_IMAGE

#ifdef GEN_TEST_IMAGE
  #define IMAGE  ""
  #ifndef WIDTH
    #define WIDTH  4032
  #endif
  #ifndef HEIGHT
    #define HEIGHT 3024
  #endif
#else
  #define IMAGE  "img/fuerte_ship.jpg"
  #define WIDTH  4032
  #define HEIGHT 3024
#endif

int main(int argc, char *argv[]) {
#ifdef HIPAVX_OUTPUT_FILENAME
    set_output_filename(HIPAVX_OUTPUT_FILENAME);
#endif
    vx_context context = vxCreateContext();

    vx_status status = VX_FAILURE;

    if (context) {
        vx_graph graph = vxCreateGraph(context);

        vx_image img[] = {
          vxCreateImageFromFile(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8, IMAGE),
          vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16),
          vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16)
        };

        if (graph) {
            vx_node nodes[] = {
                vxSobel3x3Node(graph, img[0], img[1], img[2]),
            };
        }

        status = vxVerifyGraph(graph);

        if (status == VX_SUCCESS) {
            vxWriteImageAfterGraphCompletion(graph, img[1], "./vxSobel3x3Node_1.png");
            vxWriteImageAfterGraphCompletion(graph, img[2], "./vxSobel3x3Node_2.png");
            status = vxProcessGraph(graph);
        } else {
            printf("VERIFICATION ERROR: %d\n", status);
        }

	    for (int i = 0; i < 3; i++) vxReleaseImage(&img[i]);
    }
    vxReleaseContext(&context);

    return status;
}
