#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "VX/vx.h"
#include "VX/vx_compatibility.h"
#include "hipaVX/domVX_extensions.hpp"


int main(int argc, char *argv[])
{
#ifdef HIPAVX_OUTPUT_FILENAME
  set_output_filename(HIPAVX_OUTPUT_FILENAME);
#endif
  vx_status status = VX_SUCCESS;
  vx_uint32 width = 2048;
  vx_uint32 height = 2048;
  vx_uint32 th = 40;
  vx_uint32 i;

  vx_context context = vxCreateContext();
  if (context)
  {

    vx_graph graph = vxCreateGraph(context);

    // Images
    vx_image img[] = {
	    //vxCreateImageFromFile(context, width, height, VX_DF_IMAGE_UYVY, "akif-200x300_bw.png"),
			vxCreateImage(context, width, height, VX_DF_IMAGE_RGBX),
      //vxCreateImage(context, width, height, VX_DF_IMAGE_UYVY),
      vxCreateImage(context, width, height, VX_DF_IMAGE_U8),
    };

    vx_image virt[] = {
      vxCreateVirtualImage(graph, width, height, VX_DF_IMAGE_U8),
      vxCreateVirtualImage(graph, width, height, VX_DF_IMAGE_U8),
      vxCreateVirtualImage(graph, width, height, VX_DF_IMAGE_S16),
      vxCreateVirtualImage(graph, width, height, VX_DF_IMAGE_S16),
      vxCreateVirtualImage(graph, width, height, VX_DF_IMAGE_S16),
    };


    // Constants
    vx_threshold thresh = vxCreateThresholdForImage(context, VX_THRESHOLD_TYPE_BINARY, VX_TYPE_UINT8, VX_TYPE_UINT8);
    vxSetThresholdAttribute(thresh, VX_THRESHOLD_ATTRIBUTE_THRESHOLD_VALUE, &th, sizeof(th));

    if (graph)
    {
      // Nodes
      vx_node nodes[] = {
        //vxChannelExtractNode(graph, img[0], VX_CHANNEL_Y, virt[0]),
        vxChannelExtractNode(graph, img[0], VX_CHANNEL_R, virt[0]),
        vxGaussian3x3Node(graph, virt[0], virt[1]),
        vxSobel3x3Node(graph, virt[1], virt[2], virt[3]),
        vxMagnitudeNode(graph, virt[2], virt[2], virt[4]),
        vxThresholdNode(graph, virt[4], thresh, img[1]),
      };

      // GRAPH VERIFICATION
      status = vxVerifyGraph(graph);

      if(status != VX_SUCCESS)
        printf("VERIFICATION ERROR: %d\n", status);

      if (status == VX_SUCCESS)
      {
        // WRITE RESULT
        vxWriteImageAfterGraphCompletion(graph, img[1], "result.png");

        // GRAPH EXECUTION
        status = vxProcessGraph(graph);
      }

    }
  }
  return status;
}

