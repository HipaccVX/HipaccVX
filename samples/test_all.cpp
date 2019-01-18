
#include <iostream>
#include <iterator>
#include <vector>

#include "VX/vx.h"
#include "VX/vx_compatibility.h"
#include "VX/hipaVX/domVX_extensions.hpp"

#define WIDTH  200
#define HEIGHT 300

/*
#include "VX/hipaVX/hipaVX_internal.hpp"
using namespace secret;
//Secret
int main()
{
	auto zero = new Constant<float>();
	zero->value = 0.f;
	auto one = new Constant<float>();
	one->value = 1.f;
	auto two = new Constant<float>();
	two->value = 2.f;
	auto one_half = new Div();
	one_half->subnodes[0] = one;
	one_half->subnodes[1] = two;
	auto neg_one_half = new Sub();
	neg_one_half->subnodes[0] = zero;
	neg_one_half->subnodes[1] = one_half;

	auto y = new Variable();
	y->name = "y";
	auto sigma_y = new Variable();
	sigma_y->name = "sigma_y";
	auto y_square = new Square();
	y_square->subnodes[0] = y;
	auto sigma_y_square = new Square();
	sigma_y_square->subnodes[0] = sigma_y;
	auto some_div = new Div();
	some_div->subnodes[0] = y_square;
	some_div->subnodes[1] = sigma_y_square;

	auto some_mul = new Mul();
	some_mul->subnodes[0] = neg_one_half;
	some_mul->subnodes[1] = some_div;

	auto exp = new Exp();
	exp->subnodes[0] = some_mul;

	auto pi = new Constant<float>();
	pi->value = 3.14159f;

	auto some_mul_2 = new Mul();
	some_mul_2->subnodes[0] = two;
	some_mul_2->subnodes[1] = pi;

	auto some_mul_3 = new Mul();
	some_mul_3->subnodes[0] = some_mul_2;
	some_mul_3->subnodes[1] = sigma_y;

	auto sqrt = new Sqrt();
	sqrt->subnodes[0] = some_mul_3;

	auto some_div_2 = new Div();
	some_div_2->subnodes[0] = one;
	some_div_2->subnodes[1] = some_mul_3;

	auto last_mul = new Mul();
	last_mul->subnodes[0] = some_div_2;
	last_mul->subnodes[1] = exp;

	auto g_1 = new Variable();
	g_1->name = "g_1";

	auto assignment = new Assignment();
	assignment->subnodes[0] = g_1;
	assignment->subnodes[1] = last_mul;

	auto g_1_def = new VariableDefinition();
	g_1_def->subnodes[0] = g_1;
	g_1_def->datatype = Datatype::FLOAT;

	auto statements = new Statements();
	statements->statements.emplace_back(g_1_def);
	statements->statements.emplace_back(assignment);

	std::cout << generate(statements);

	return -1;
}*/

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

			vxCreateImage(context, WIDTH/4, HEIGHT/3, VX_DF_IMAGE_U8),             /*25: scale */
			vxCreateImage(context, WIDTH*4, HEIGHT*3, VX_DF_IMAGE_U8),             /*26: scale */

			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_RGBX),               /*27: RGBA reconstructed */
			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_RGBX),               /*28: GBRA reconstructed */

			vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),				   /*29: another bilateralfilter */
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

				vxHipaccNode(graph, "hipacc_interop/BilateralFilter.cpp", bilateral_parameters.data(), bilateral_parameters.size(), images[24]),
				vxFWriteImageNode(graph, images[24], "akif-200x300_bw_customhipacc_bilateral.png"),

				/*vxScaleImageNode(graph, images[0], images[25], VX_INTERPOLATION_NEAREST_NEIGHBOR),
				vxFWriteImageNode(graph, images[25], "akif-200x300_bw_scaled_down.png"),
				vxScaleImageNode(graph, images[0], images[26], VX_INTERPOLATION_NEAREST_NEIGHBOR),
				vxFWriteImageNode(graph, images[26], "akif-200x300_bw_scaled_up.png"),*/

				vxChannelCombineNode(graph, images[21], images[20], images[19], images[22], images[27]),
				vxFWriteImageNode(graph, images[27], "akif-200x300_rgba_rgba.png"),
				vxChannelCombineNode(graph, images[20], images[19], images[21], images[22], images[28]),
				vxFWriteImageNode(graph, images[28], "akif-200x300_rgba_gbra.png"),

				vxAnotherBilateralFilterNode(graph, images[0], 16, images[29]),
				vxFWriteImageNode(graph, images[29], "akif-200x300_bw_another_bilateral.png"),
			};

            //Step4.Verify Graph
            status = vxVerifyGraph(graph);
            if (status == VX_SUCCESS)
            {
                //Step5. Process Graph
                status = vxProcessGraph(graph);
            }

			vxDrawDotGraph(graph, "graph.dot", 0);

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

