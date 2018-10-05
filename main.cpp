
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

			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /*10: absdiff */
			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /*11: add */
			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /*12: sub */

			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /*13: Box_3x3 */

			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /*14: dilate */
			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /*15: erode */

			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /*16: gaussian */
		};

		int32_t two = 2;
		vx_scalar two_scalar = vxCreateScalar(context, VX_TYPE_INT32, (void*) &two);

		//0.0005f, 5.0f, 0.04f
		float harris_strength_f = 0.00005f;
		vx_scalar harris_strength = vxCreateScalar(context, VX_TYPE_FLOAT32, (void*) &harris_strength_f);
		float harris_min_distance_f = 5.0f;
		vx_scalar harris_min_distance = vxCreateScalar(context, VX_TYPE_FLOAT32, (void*) &harris_min_distance_f);
		float harris_sensitivity_f = 0.04f;
		vx_scalar harris_sensitivity = vxCreateScalar(context, VX_TYPE_FLOAT32, (void*) &harris_sensitivity_f);
		vx_array harris_corners = vxCreateArray(context, VX_TYPE_KEYPOINT, 10);
		vx_scalar harris_corners_num = vxCreateScalar(context, VX_TYPE_INT32, (void*) &two);

		//Step 2.Create Graph
		vx_graph graph = vxCreateGraph(context);
		if (vxGetStatus((vx_reference)graph) == VX_SUCCESS)
		{
			printf("graph has been created... \n");
			//Step 3.Create node and add into a graph
			vx_node nodes[] = {
				/*vxSobel3x3Node(graph, images[0], images[1], images[2]),
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

				vxAbsDiffNode(graph, images[0], images[5], images[10]),
				vxFWriteImageNode(graph, images[10], "akif-200x300_bw_abs_diff.png"),

				vxAddNode(graph, images[0], images[5], VX_CONVERT_POLICY_WRAP, images[11]),
				vxSubtractNode(graph, images[11], images[5], VX_CONVERT_POLICY_WRAP, images[12]),
				vxFWriteImageNode(graph, images[12], "akif-200x300_bw_add_sub.png"),


				vxDilate3x3Node(graph, images[0], images[14]),
				vxFWriteImageNode(graph, images[14], "akif-200x300_bw_dilate.png"),
				vxErode3x3Node(graph, images[0], images[15]),
				vxFWriteImageNode(graph, images[15], "akif-200x300_bw_erode.png"),

				vxBox3x3Node(graph, images[0], images[13]),
				vxFWriteImageNode(graph, images[13], "akif-200x300_bw_box.png"),
				vxGaussian3x3Node(graph, images[0], images[16]),*/
				vxFWriteImageNode(graph, images[16], "akif-200x300_bw_gaussian.png"),/**/

				vxHarrisCornersNode(graph, images[0], harris_strength, harris_min_distance,
									harris_sensitivity, 3, 3, harris_corners, harris_corners_num),
				//vxFWriteImageNode(graph, images[17], "akif-200x300_bw_harris.png"),
			};

            //Step4.Verify Graph
            status = vxVerifyGraph(graph);
            if (status == VX_SUCCESS)
            {
                //Step5. Process Graph
                status = vxProcessGraph(graph);
            }

			/*for (i = 0; i < 8; i++)
            {
                vxReleaseNode(&nodes[i]);
			}*/

            vxReleaseGraph(&graph);
		}

		for (i = 0; i < 5; i++)
        {
            vxReleaseImage(&images[i]);
        }
        vxReleaseContext(&context);
	}
}

