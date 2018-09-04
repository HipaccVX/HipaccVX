
#include <iostream>
#include <iterator>

#include "VX/vx.h"
#include "VX/hipaVX/hipaVX_extension.hpp"

#define WIDTH  200
#define HEIGHT 300


int main()
{
	vx_status status = VX_FAILURE;
	//Step 1.Create Context
	vx_context context = vxCreateContext();

	if (vxGetStatus((vx_reference)context) == VX_SUCCESS)
	{
		vx_uint32 i = 0, w = 5184, h = 3456;
		//Step 2.Create Image Object
		vx_image images[] = {
			vxCreateImageFromFile(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8, "akif-200x300_bw.png"),                 /* 0: input */
			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16),                /* 1: sobel_X */
			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16),                /* 2: sobel_Y */
			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16),                /* 3: sobel_mag */
			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /* 4: out */

			vxCreateImageFromFile(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8, "random_in.png"),
			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /* 6: AND */
			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /* 7: OR */

			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /* 8: XOR */
			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /* 9: XOR_XOR */
		};

		int32_t two = 2;

		vx_scalar two_scalar = vxCreateScalar(context, VX_TYPE_INT32, (void*) &two);

		//Step 2.Create Graph
		vx_graph graph = vxCreateGraph(context);
		if (vxGetStatus((vx_reference)graph) == VX_SUCCESS)
		{
			printf("graph has been created... \n");
			//Step 3.Create node and add into a graph
			vx_node nodes[] = {
				vxSobel3x3Node(graph, images[0], images[1], images[2]),
				vxMagnitudeNode(graph, images[1], images[2], images[3]),
				vxConvertDepthNode(graph, images[3], images[4], VX_CONVERT_POLICY_SATURATE, two_scalar),

				vxFWriteImageNode(graph, images[1], "akif-200x300_bw_sobel_x.png"),
				vxFWriteImageNode(graph, images[2], "akif-200x300_bw_sobel_y.png"),
				vxFWriteImageNode(graph, images[3], "akif-200x300_bw_mag.png"),
				vxFWriteImageNode(graph, images[4], "akif-200x300_bw_out.png"),


				vxAndNode(graph, images[4], images[5], images[6]),
				vxOrNode(graph, images[4], images[6], images[7]),
				vxFWriteImageNode(graph, images[7], "akif-200x300_bw_out_and_or.png"),

				vxXorNode(graph, images[4], images[5], images[8]),
				vxXorNode(graph, images[8], images[5], images[9]),
				vxFWriteImageNode(graph, images[9], "akif-200x300_bw_xor_xor.png"),
			};

            //Step4.Verify Graph
            status = vxVerifyGraph(graph);
            if (status == VX_SUCCESS)
            {
                //Step5. Process Graph
                status = vxProcessGraph(graph);
            }

			for (i = 0; i < 8; i++)
            {
                vxReleaseNode(&nodes[i]);
            }

            vxReleaseGraph(&graph);
		}

		for (i = 0; i < 5; i++)
        {
            vxReleaseImage(&images[i]);
        }
        vxReleaseContext(&context);
	}
}

