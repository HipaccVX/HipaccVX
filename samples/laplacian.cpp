
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

		vx_image src = vxCreateImageFromFile(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8, "akif-200x300_bw.png");
		vx_image dst = vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8);


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

		//Step 2.Create Graph
		vx_graph graph = vxCreateGraph(context);
		if (vxGetStatus((vx_reference)graph) == VX_SUCCESS)
		{
			//Step 3.Create node and add into a graph
			vx_node laplacian_node = vxConvolveNode(graph, src, laplacian_convolution, dst);

			vx_node out_node_write = vxFWriteImageNode(graph, dst, "laplacian.png");

            //Step4.Verify Graph
            status = vxVerifyGraph(graph);
            if (status == VX_SUCCESS)
            {
                //Step5. Process Graph
                status = vxProcessGraph(graph);
            }

			vxDrawDotGraph(graph, "graph.dot", 0);

            vxReleaseGraph(&graph);
		}

        vxReleaseContext(&context);
	}
}

