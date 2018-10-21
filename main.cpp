
#include <iostream>
#include <iterator>
#include <vector>

#include "VX/vx.h"
#include "VX/vx_compatibility.h"
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

			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /*17: thresh */

			vxCreateImageFromFile(context, WIDTH, HEIGHT, VX_DF_IMAGE_RGBX, "akif-200x300_rgba.png"), /*18: thresh */
			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /*19: r */
			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /*20: g */
			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /*21: b */
			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /*22: a */

			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /*23: custom convolve scharr_x */

			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),                 /*24: custom hipacc kernel bilateral */
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
		vx_array harris_corners = vxCreateArray(context, VX_TYPE_KEYPOINT, 100);
		vx_scalar harris_corners_num = vxCreateScalar(context, VX_TYPE_INT32, (void*) &two);

		vx_threshold thresh = vxCreateThresholdForImage(context, VX_THRESHOLD_TYPE_RANGE, VX_DF_IMAGE_U8, VX_DF_IMAGE_U8);
		vx_int32 some_int = 100;
		vxSetThresholdAttribute(thresh, VX_THRESHOLD_THRESHOLD_LOWER, &some_int, sizeof(some_int));
		some_int = 200;
		vxSetThresholdAttribute(thresh, VX_THRESHOLD_THRESHOLD_UPPER, &some_int, sizeof(some_int));

		vx_convolution scharr_x = vxCreateConvolution(context, 3, 3);
		vx_int16 gx[3][3] = {
				{  3, 0, -3},
				{ 10, 0,-10},
				{  3, 0, -3},
			};
		vx_uint32 scale = 2;
		vxCopyConvolutionCoefficients(scharr_x, (vx_int16*)gx, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);
		vxSetConvolutionAttribute(scharr_x, VX_CONVOLUTION_SCALE, &scale, sizeof(scale));

		const float coef_bilateral[25] = {
			 0.018316f, 0.082085f, 0.135335f, 0.082085f, 0.018316f ,
			 0.082085f, 0.367879f, 0.606531f, 0.367879f, 0.082085f ,
			 0.135335f, 0.606531f, 1.000000f, 0.606531f, 0.135335f ,
			 0.082085f, 0.367879f, 0.606531f, 0.367879f, 0.082085f ,
			 0.018316f, 0.082085f, 0.135335f, 0.082085f, 0.018316f
		};
		vx_matrix sigma_s = vxCreateMatrix(context, VX_TYPE_FLOAT32, 5, 5);
		vxCopyMatrix(sigma_s, (void*)coef_bilateral, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

		int32_t sixteen = 16;
		vx_scalar sigma_r = vxCreateScalar(context, VX_TYPE_INT32, &sixteen);

		std::vector<vx_reference> bilateral_parameters;
		bilateral_parameters.push_back((vx_reference) images[0]);
		bilateral_parameters.push_back((vx_reference) sigma_s);
		bilateral_parameters.push_back((vx_reference) sigma_r);

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
				vxGaussian3x3Node(graph, images[0], images[16]),
				vxFWriteImageNode(graph, images[16], "akif-200x300_bw_gaussian.png"),

				vxHarrisCornersNode(graph, images[0], harris_strength, harris_min_distance,
									harris_sensitivity, 3, 3, harris_corners, harris_corners_num),
				//vxFWriteImageNode(graph, images[17], "akif-200x300_bw_harris.png"),

				vxThresholdNode(graph, images[0], thresh, images[17]),
				vxFWriteImageNode(graph, images[17], "akif-200x300_bw_thresh.png"),


				vxChannelExtractNode(graph, images[18], VX_CHANNEL_R, images[19]),
				vxChannelExtractNode(graph, images[18], VX_CHANNEL_G, images[20]),
				vxChannelExtractNode(graph, images[18], VX_CHANNEL_B, images[21]),
				vxChannelExtractNode(graph, images[18], VX_CHANNEL_A, images[22]),
				vxFWriteImageNode(graph, images[19], "akif-200x300_rgba_r.png"),
				vxFWriteImageNode(graph, images[20], "akif-200x300_rgba_g.png"),
				vxFWriteImageNode(graph, images[21], "akif-200x300_rgba_b.png"),
				vxFWriteImageNode(graph, images[22], "akif-200x300_rgba_a.png"),

				vxConvolveNode(graph, images[0], scharr_x, images[23]),
				vxFWriteImageNode(graph, images[23], "akif-200x300_bw_scharr_x.png"),

				vxHipaccNode(graph, "external_kernels/BilateralFilter.cpp", bilateral_parameters.data(), bilateral_parameters.size(), images[24]),
				vxFWriteImageNode(graph, images[24], "akif-200x300_bw_customhipacc_bilateral.png"),
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

