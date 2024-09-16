
#include <iostream>
#include <iterator>
#include <vector>

#include "VX/vx.h"
#include "VX/vx_compatibility.h"
#include "hipaVX/domVX_extensions.hpp"
#include "hipaVX/vx_adaptor.hpp"

#define WIDTH 200
#define HEIGHT 300

int main() {

  vx_status status = VX_FAILURE;
  // Step 1.Create Context
  vx_context context = vxCreateContext();

  if (vxGetStatus((vx_reference)context) == VX_SUCCESS) {
    vx_uint32 i = 0, w = 5184, h = 3456;
    // Step 2.Create Image Object
    vx_image images[] = {
        vxCreateImageFromFile(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8,
                              "sample_images/200x300_bw_1.png"), /* 0: input */
        vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16), /* 1: sobel_X */
        vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16), /* 2: sobel_Y */
        vxCreateImage(context, WIDTH, HEIGHT,
                      VX_DF_IMAGE_S16), /* 3: sobel_mag */
        vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /* 4: out */
    };

   
    // Step 2.Create Graph
    vx_graph graph = vxCreateGraph(context);


    if (vxGetStatus((vx_reference)graph) == VX_SUCCESS) {
      printf("graph has been created... \n");
      vx_node nodes[] = {
          vxSobel3x3Node(graph, images[0], images[1], images[2]),
          vxMagnitudeNode(graph, images[1], images[2], images[3]),
      };

      vxWriteImageAfterGraphCompletion(graph, images[0],
                                       "sample_images/200x300_bw_1_done_nothing.png");
      vxWriteImageAfterGraphCompletion(graph, images[1],
                                       "sample_images/200x300_bw_1_sobel_x.png");
      vxWriteImageAfterGraphCompletion(graph, images[2],
                                       "sample_images/200x300_bw_1_sobel_y.png");
      vxWriteImageAfterGraphCompletion(graph, images[3],
                                       "sample_images/200x300_bw_1_bw_mag.png");

      // Step4.Verify Graph
      status = vxVerifyGraph(graph);
      if (status == VX_SUCCESS) {
        // Step5. Process Graph
        status = vxProcessGraph(graph);
      }



      vxReleaseGraph(&graph);
    }

    vxReleaseContext(&context);
  }
}
