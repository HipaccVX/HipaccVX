#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "VX/vx.h"
#include "VX/vx_compatibility.h"
#include "VX/hipaVX/hipaVX_extension.hpp"


int main(int argc, char *argv[])
{
  vx_status status = VX_SUCCESS;
  vx_uint32 width = 640;
  vx_uint32 height = 480;
  vx_uint32 th = 40;
  vx_uint32 i;
  
  vx_context context = vxCreateContext();
  if (context)
  {
    
    vx_graph graph = vxCreateGraph(context);
    
    // Images
    vx_image images[] = {
	  vxCreateImageFromFile(context, width, height, VX_DF_IMAGE_U8, "akif-200x300_bw.png"),
      vxCreateImage(context, width, height,  VX_DF_IMAGE_U8),
      vxCreateImage(context, width, height, VX_DF_IMAGE_S16),
      vxCreateImage(context, width, height, VX_DF_IMAGE_S16),
      vxCreateImage(context, width, height, VX_DF_IMAGE_U8),
      vxCreateImage(context, width, height, VX_DF_IMAGE_U8),
    };
    
    
    // Constants
    vx_threshold thresh = vxCreateThresholdForImage(context, VX_THRESHOLD_TYPE_BINARY, VX_TYPE_UINT8, VX_TYPE_UINT8);
    vxSetThresholdAttribute(thresh, VX_THRESHOLD_ATTRIBUTE_THRESHOLD_VALUE, &th, sizeof(th));
    
    if (graph)
    {
      // Nodes
      vx_node nodes[] = {
        vxGaussian3x3Node(graph, images[0], images[1]),
        vxSobel3x3Node(graph, images[1], images[2], images[3]),
        vxMagnitudeNode(graph, images[2], images[3], images[4]),
        vxThresholdNode(graph, images[4], thresh, images[5]),
      };
      
      // GRAPH VERIFICATION
      status = vxVerifyGraph(graph);
      
      if(status != VX_SUCCESS)
        printf("VERIFICATION ERROR: %d\n", status);
      
      if (status == VX_SUCCESS)
      {
        // GRAPH EXECUTION
        status = vxProcessGraph(graph);
        
        // WRITE RESULT
		vxFWriteImageNode(graph, images[5], "result.png");
      }
      
    }
  }
  return status;
}

