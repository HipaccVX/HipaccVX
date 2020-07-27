
#include <iostream>
#include <iterator>
#include <vector>

#include "VX/vx.h"
#include "VX/vx_compatibility.h"
#include "hipaVX/domVX_extensions.hpp"

#ifndef WIDTH
  #define WIDTH  4032
#endif
#ifndef HEIGHT
  #define HEIGHT 3024
#endif


int main()
{
#ifdef HIPAVX_OUTPUT_FILENAME
	set_output_filename(HIPAVX_OUTPUT_FILENAME);
#endif
	vx_status status = VX_FAILURE;
	//Step 1.Create Context
	vx_context context = vxCreateContext();

	vx_float32 scale = 1;
	vx_scalar s_scale = vxCreateScalar(context, VX_TYPE_FLOAT32, &scale);

	vx_enum convtP = static_cast<vx_enum>(VX_CONVERT_POLICY_SATURATE);
	vx_enum rounde = static_cast<vx_enum>(VX_ROUND_POLICY_TO_ZERO);

	vx_int32 shift = 0;
	vx_scalar s_shift = vxCreateScalar(context, VX_TYPE_INT32, &shift);

	vx_uint32 th = 250;
	vx_threshold thresh = vxCreateThresholdForImage(context, VX_THRESHOLD_TYPE_BINARY, VX_DF_IMAGE_U8, VX_DF_IMAGE_U8);
	vxSetThresholdAttribute(thresh, VX_THRESHOLD_THRESHOLD_VALUE, &th, sizeof(th));

	if (vxGetStatus((vx_reference)context) == VX_SUCCESS)
	{
		vx_image src = vxCreateImageFromFile(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8, "beach.jpg");
		vx_image sx16_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16);
		vx_image sy16_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16);
		vx_image sx_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8);
		vx_image sy_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8);
		vx_image sxx_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8);
		vx_image syy_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8);
		vx_image sxy_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8);
		vx_image gxx_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8);
		vx_image gyy_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8);
		vx_image gxy_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8);
		vx_image gradX_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8);
		vx_image gradY_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8);
		vx_image gradX16_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16);
		vx_image gradY16_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16);
		vx_image grad_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16);
		vx_image lambdaL_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16);
		vx_image lambda1_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16);
		vx_image lambda18_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8);
		vx_image lambda2_img = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16);
		vx_image dst = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8);

		//Step 2.Create Graph
		vx_graph graph = vxCreateGraph(context);
		if (vxGetStatus((vx_reference)graph) == VX_SUCCESS)
		{
			//Step 3.Create node and add into a graph
			vx_node sobel_node = vxSobel3x3Node(graph, src, sx16_img, sy16_img);
			vx_node convX_node = vxConvertDepthNode(graph, sx16_img, sx_img, convtP, s_shift);
			vx_node convY_node = vxConvertDepthNode(graph, sy16_img, sy_img, convtP, s_shift);
			vx_node mult_node = vxMultiplyNode(graph, sx_img, sx_img, s_scale, convtP, rounde, sxx_img );
			vx_node squareY_node = vxMultiplyNode(graph, sy_img, sy_img, s_scale, convtP, rounde, syy_img );
			vx_node squareXY_node = vxMultiplyNode(graph, sx_img, sy_img, s_scale, convtP, rounde, sxy_img );
			vx_node gaussianX_node = vxGaussian3x3Node(graph, sxx_img, gxx_img);
			vx_node gaussianY_node = vxGaussian3x3Node(graph, syy_img, gyy_img);
			vx_node gaussianXY_node = vxGaussian3x3Node(graph, sxy_img, gxy_img);
			vx_node gradX_node = vxSubtractNode(graph, gxx_img, gyy_img, convtP, gradX_img);
			vx_node gradY_node = vxAddNode(graph, gxy_img, gxy_img, convtP, gradY_img);
			vx_node convgX_node = vxConvertDepthNode(graph, gradX_img, gradX16_img, convtP, s_shift);
			vx_node convgY_node = vxConvertDepthNode(graph, gradY_img, gradY16_img, convtP, s_shift);
			vx_node mag_node = vxMagnitudeNode(graph, gradX16_img, gradY16_img, grad_img);
			vx_node lambdaL_node = vxAddNode(graph, gxx_img, gyy_img, convtP, lambdaL_img);
			vx_node lambda1_node = vxAddNode(graph, lambdaL_img, grad_img, convtP, lambda1_img);
			vx_node lambda2_node = vxSubtractNode(graph, lambdaL_img, grad_img, convtP, lambda2_img);
			vx_node convLam_node = vxConvertDepthNode(graph, lambda1_img, lambda18_img, convtP, s_shift);
			vx_node out_node = vxThresholdNode(graph, lambda18_img, thresh, dst);

			vxWriteImageAfterGraphCompletion(graph, dst, "tomasi.png");

            //Step4.Verify Graph
            status = vxVerifyGraph(graph);
            if (status == VX_SUCCESS)
            {
                //Step5. Process Graph
                status = vxProcessGraph(graph);
            }

            vxReleaseGraph(&graph);
		}

        vxReleaseContext(&context);
	}
}

