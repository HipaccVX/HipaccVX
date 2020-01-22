
#include <iostream>
#include <iterator>
#include <vector>

#include "VX/vx.h"
#include "VX/vx_compatibility.h"
#include "hipaVX/domVX_extensions.hpp"
#include "hipaVX/dsl/api.hpp"
#include "hipaVX/vx_adaptor.hpp"

#define WIDTH 200
#define HEIGHT 300

int main() {
  // AST, output single, input[asd]
  /*function_ast::Statements ast_fun(3);
  ast_fun << assign(ast_fun[0], ast_fun[1] * (ast_fun[2] + ast_fun[1]));

  std::cout << "AST Generation: \n";
  std::cout << ast_fun.generate_source();
  std::cout << "\n";


  //DomVX

  auto image_i_1 = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
  auto image_i_2 = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
  auto image_o = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);


  auto t = std::make_shared<DomVX::MapTest>();
  t->function = ast_fun;
  t->register_image({image_o, image_i_1, image_i_2});

  std::cout << "DomVX Generation: \n";
  std::cout << t->generate_source();
  std::cout << "\n";

  return 0;
*/

  vx_status status = VX_FAILURE;
  // Step 1.Create Context
  vx_context context = vxCreateContext();

  if (vxGetStatus((vx_reference)context) == VX_SUCCESS) {
	vx_uint32 i = 0, w = 5184, h = 3456;
	// Step 2.Create Image Object
	vx_image images[] = {
		vxCreateImageFromFile(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8,
							  "akif-200x300_bw.png"),           /* 0: input */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16), /* 1: sobel_X */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16), /* 2: sobel_Y */
		vxCreateImage(context, WIDTH, HEIGHT,
					  VX_DF_IMAGE_S16), /* 3: sobel_mag */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /* 4: out */

		vxCreateImageFromFile(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8,
							  "random_in.png"),
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /* 6: AND */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /* 7: OR */

		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /* 8: XOR */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /* 9: XOR_XOR */

		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /*10: absdiff */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /*11: add */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /*12: sub */

		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /*13: Box_3x3 */

		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /*14: dilate */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /*15: erode */

		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /*16: gaussian */

		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /*17: thresh */

		vxCreateImageFromFile(context, WIDTH, HEIGHT, VX_DF_IMAGE_RGBX,
							  "akif-200x300_rgba.png"),        /*18: thresh */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /*19: r */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /*20: g */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /*21: b */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8), /*22: a */

		vxCreateImage(context, WIDTH, HEIGHT,
					  VX_DF_IMAGE_U8), /*23: custom convolve scharr_x */

		vxCreateImage(context, WIDTH, HEIGHT,
					  VX_DF_IMAGE_U8), /*24: custom hipacc kernel bilateral */

		vxCreateImage(context, WIDTH / 4, HEIGHT / 3,
					  VX_DF_IMAGE_U8), /*25: scale */
		vxCreateImage(context, WIDTH * 4, HEIGHT * 3,
					  VX_DF_IMAGE_U8), /*26: scale */

		vxCreateImage(context, WIDTH, HEIGHT,
					  VX_DF_IMAGE_RGBX), /*27: RGBA reconstructed */
		vxCreateImage(context, WIDTH, HEIGHT,
					  VX_DF_IMAGE_RGBX), /*28: GBRA reconstructed */

		vxCreateImage(context, WIDTH, HEIGHT,
					  VX_DF_IMAGE_U8), /*29: another bilateralfilter */

		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16), /*30: test */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16), /*31: test */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16), /*32: test */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_S16), /*33: test */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),  /*34: test */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),  /*35: test */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),  /*36: test */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),  /*37: copy */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),  /*38: nonmax */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),  /*39: median */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),  /*40: cpp test node */
		vxCreateImage(context, WIDTH, HEIGHT, VX_DF_IMAGE_U8),  /*41: median after cpp test node */
	};

	int32_t two = 2;
	vx_scalar two_scalar = vxCreateScalar(context, VX_TYPE_INT32, (void*)&two);

	// 0.0005f, 5.0f, 0.04f
	float harris_strength_f = 0.00005f;
	vx_scalar harris_strength =
		vxCreateScalar(context, VX_TYPE_FLOAT32, (void*)&harris_strength_f);
	float harris_min_distance_f = 5.0f;
	vx_scalar harris_min_distance =
		vxCreateScalar(context, VX_TYPE_FLOAT32, (void*)&harris_min_distance_f);
	float harris_sensitivity_f = 0.04f;
	vx_scalar harris_sensitivity =
		vxCreateScalar(context, VX_TYPE_FLOAT32, (void*)&harris_sensitivity_f);
	vx_array harris_corners = vxCreateArray(context, VX_TYPE_KEYPOINT, 100);
	vx_scalar harris_corners_num =
		vxCreateScalar(context, VX_TYPE_INT32, (void*)&two);

	vx_threshold thresh = vxCreateThresholdForImage(
		context, VX_THRESHOLD_TYPE_RANGE, VX_DF_IMAGE_U8, VX_DF_IMAGE_U8);
	vx_int32 some_int = 100;
	vxSetThresholdAttribute(thresh, VX_THRESHOLD_THRESHOLD_LOWER, &some_int,
							sizeof(some_int));
	some_int = 200;
	vxSetThresholdAttribute(thresh, VX_THRESHOLD_THRESHOLD_UPPER, &some_int,
							sizeof(some_int));

	vx_threshold thresh_bin = vxCreateThresholdForImage(
		context, VX_THRESHOLD_TYPE_BINARY, VX_DF_IMAGE_U8, VX_DF_IMAGE_U8);
	vxSetThresholdAttribute(thresh_bin, VX_THRESHOLD_THRESHOLD_VALUE, &some_int,
							sizeof(some_int));

	vx_convolution scharr_x = vxCreateConvolution(context, 3, 3);
	vx_int16 gx[3][3] = {
		{3, 0, -3},
		{10, 0, -10},
		{3, 0, -3},
	};
	vx_uint32 scale = 2;
	vxCopyConvolutionCoefficients(scharr_x, (vx_int16*)gx, VX_WRITE_ONLY,
								  VX_MEMORY_TYPE_HOST);
	vxSetConvolutionAttribute(scharr_x, VX_CONVOLUTION_SCALE, &scale,
							  sizeof(scale));

	const float coef_bilateral[25] = {
		0.018316f, 0.082085f, 0.135335f, 0.082085f, 0.018316f,
		0.082085f, 0.367879f, 0.606531f, 0.367879f, 0.082085f,
		0.135335f, 0.606531f, 1.000000f, 0.606531f, 0.135335f,
		0.082085f, 0.367879f, 0.606531f, 0.367879f, 0.082085f,
		0.018316f, 0.082085f, 0.135335f, 0.082085f, 0.018316f};
	vx_matrix sigma_s = vxCreateMatrix(context, VX_TYPE_FLOAT32, 5, 5);
	vxCopyMatrix(sigma_s, (void*)coef_bilateral, VX_WRITE_ONLY,
				 VX_MEMORY_TYPE_HOST);

	int32_t sixteen = 16;
	vx_scalar sigma_r = vxCreateScalar(context, VX_TYPE_INT32, &sixteen);

	std::vector<vx_reference> bilateral_parameters;
	bilateral_parameters.push_back((vx_reference)images[0]);
	bilateral_parameters.push_back((vx_reference)sigma_s);
	bilateral_parameters.push_back((vx_reference)sigma_r);

	// new stuff

	auto hk = vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/bilateral.hpp");
	vxAddParameterToKernel(hk, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
	vxAddParameterToKernel(hk, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
	vxAddParameterToKernel(hk, 2, VX_INPUT, VX_TYPE_MATRIX, 0);
	vxAddParameterToKernel(hk, 3, VX_INPUT, VX_TYPE_SCALAR, 0);

	// Step 2.Create Graph
	vx_graph graph = vxCreateGraph(context);

	/*auto hn = vxCreateGenericNode(graph, hk);
	vxSetParameterByIndex(hn, 0, (vx_reference)images[24]);
	vxSetParameterByIndex(hn, 1, (vx_reference)images[4]);
	vxSetParameterByIndex(hn, 2, (vx_reference)sigma_s);
	vxSetParameterByIndex(hn, 3, (vx_reference)sigma_r);*/


	uint8_t zero = 20;
	vx_scalar zero_scalar = vxCreateScalar(context, VX_TYPE_UINT8, &zero);

	if (vxGetStatus((vx_reference)graph) == VX_SUCCESS) {
	  printf("graph has been created... \n");
	  vx_node nodes[] = {
		  /*vxSobel3x3Node(graph, images[0], images[1], images[2]),
		  vxMagnitudeNode(graph, images[1], images[2], images[3]),
		  vxConvertDepthNode(graph, images[3], images[4],
							 VX_CONVERT_POLICY_SATURATE, two_scalar),

		  vxAndNode(graph, images[4], images[5], images[6]),
		  vxOrNode(graph, images[4], images[5], images[7]),

		  vxXorNode(graph, images[4], images[5], images[8]),
		  vxXorNode(graph, images[8], images[5], images[9]),

		  vxAbsDiffNode(graph, images[0], images[5], images[10]),

		  vxAddNode(graph, images[0], images[5], VX_CONVERT_POLICY_WRAP,
					images[11]),
		  vxSubtractNode(graph, images[11], images[5], VX_CONVERT_POLICY_WRAP,
						 images[12]),

		  vxDilate3x3Node(graph, images[0], images[14]),
		  vxErode3x3Node(graph, images[0], images[15]),

		  vxBox3x3Node(graph, images[0], images[13]),
		  vxGaussian3x3Node(graph, images[0], images[16]),

		  vxHarrisCornersNode(graph, images[0], harris_strength,
							  harris_min_distance, harris_sensitivity, 3, 3,
							  harris_corners, harris_corners_num),
		  vxThresholdNode(graph, images[0], thresh, images[17]),

		  vxChannelExtractNode(graph, images[18], VX_CHANNEL_R, images[19]),
		  vxChannelExtractNode(graph, images[18], VX_CHANNEL_G, images[20]),
		  vxChannelExtractNode(graph, images[18], VX_CHANNEL_B, images[21]),
		  vxChannelExtractNode(graph, images[18], VX_CHANNEL_A, images[22]),

		  vxConvolveNode(graph, images[0], scharr_x, images[23]),

		  /*vxScaleImageNode(graph, images[0], images[25],
		  VX_INTERPOLATION_NEAREST_NEIGHBOR),
		  vxFWriteImageNode(graph,
		  images[25], "akif-200x300_bw_scaled_down.png"),
		  vxScaleImageNode(graph, images[0], images[26],
		  VX_INTERPOLATION_NEAREST_NEIGHBOR),
		  vxFWriteImageNode(graph,
		  images[26], "akif-200x300_bw_scaled_up.png"),*/

		  /*vxChannelCombineNode(graph, images[21], images[20], images[19],
							   images[22], images[27]),
		  vxChannelCombineNode(graph, images[20], images[19], images[21],
							   images[22], images[28]),
		  /*
		   // vxAnotherBilateralFilterNode(graph, images[0], 16,
		   images[29]),
		   // vxFWriteImageNode(graph, images[29],
		   // "akif-200x300_bw_another_bilateral.png"),/**/
		  /*vxAddNode(graph, images[1], images[2], VX_CONVERT_POLICY_WRAP,
					images[30]),
		  vxAddNode(graph, images[6], images[1], VX_CONVERT_POLICY_WRAP,
					images[31]),
		  vxAddNode(graph, images[7], images[6], VX_CONVERT_POLICY_WRAP,
					images[34]),
		  vxSubtractNode(graph, images[10], images[6], VX_CONVERT_POLICY_WRAP,
						 images[32]),
		  vxSubtractNode(graph, images[1], images[10], VX_CONVERT_POLICY_WRAP,
						 images[33]),
		  vxSubtractNode(graph, images[6], images[7], VX_CONVERT_POLICY_WRAP,
						 images[35]),
		  vxCopyNode(graph, (vx_reference)images[0], (vx_reference)images[37]),
		  vxNonMaxSuppressionNode(graph, images[4], nullptr, 5, images[38]),*/
		  vxMedian3x3Node(graph, images[0], images[39]),
		  testNode(graph, images[39], zero_scalar, images[40]),
		  vxMedian3x3Node(graph, images[40], images[41]),
	  };

	  /*vxWriteImageAfterGraphCompletion(graph, images[0],
									   "akif-200x300_bw_done_nothing.png");
	  vxWriteImageAfterGraphCompletion(graph, images[1],
									   "akif-200x300_bw_sobel_x.png");
	  vxWriteImageAfterGraphCompletion(graph, images[2],
									   "akif-200x300_bw_sobel_y.png");
	  vxWriteImageAfterGraphCompletion(graph, images[3],
									   "akif-200x300_bw_mag.png");
	  vxWriteImageAfterGraphCompletion(graph, images[4],
									   "akif-200x300_bw_out.png");
	  vxWriteImageAfterGraphCompletion(graph, images[7],
									   "akif-200x300_bw_out_and_or.png");
	  vxWriteImageAfterGraphCompletion(graph, images[9],
									   "akif-200x300_bw_xor_xor.png");
	  vxWriteImageAfterGraphCompletion(graph, images[10],
									   "akif-200x300_bw_abs_diff.png");
	  vxWriteImageAfterGraphCompletion(graph, images[12],
									   "akif-200x300_bw_add_sub.png");
	  vxWriteImageAfterGraphCompletion(graph, images[14],
									   "akif-200x300_bw_dilate.png");
	  vxWriteImageAfterGraphCompletion(graph, images[15],
									   "akif-200x300_bw_erode.png");
	  vxWriteImageAfterGraphCompletion(graph, images[13],
									   "akif-200x300_bw_box.png");
	  vxWriteImageAfterGraphCompletion(graph, images[16],
									   "akif-200x300_bw_gaussian.png");
	  vxWriteImageAfterGraphCompletion(graph, images[17],
									   "akif-200x300_bw_thresh.png");
	  vxWriteImageAfterGraphCompletion(graph, images[19],
									   "akif-200x300_rgba_r.png");
	  vxWriteImageAfterGraphCompletion(graph, images[20],
									   "akif-200x300_rgba_g.png");
	  vxWriteImageAfterGraphCompletion(graph, images[21],
									   "akif-200x300_rgba_b.png");
	  vxWriteImageAfterGraphCompletion(graph, images[22],
									   "akif-200x300_rgba_a.png");
	  vxWriteImageAfterGraphCompletion(graph, images[27],
									   "akif-200x300_rgba_rgba.png");
	  vxWriteImageAfterGraphCompletion(graph, images[28],
									   "akif-200x300_rgba_gbra.png");
	  vxWriteImageAfterGraphCompletion(graph, images[37],
									   "akif-200x300_bw_copy.png");
	  vxWriteImageAfterGraphCompletion(graph, images[23],
									   "akif-200x300_bw_scharr_x.png");
	  vxWriteImageAfterGraphCompletion(graph, images[38],
									   "akif-200x300_bw_non_max.png");*/
	  vxWriteImageAfterGraphCompletion(graph, images[39],
									   "akif-200x300_bw_median.png");
	  vxWriteImageAfterGraphCompletion(graph, images[40],
									   "akif-200x300_bw_cpp_test.png");
	  vxWriteImageAfterGraphCompletion(graph, images[41],
									   "akif-200x300_bw_cpp_test_then_median.png");

	  // Step4.Verify Graph
	  status = vxVerifyGraph(graph);
	  if (status == VX_SUCCESS) {
		// Step5. Process Graph
		status = vxProcessGraph(graph);
	  }

	  // vxDrawDotGraph(graph, "graph.dot", 0);

	  for (i = 0; i < 8; i++) {
		vxReleaseNode(&nodes[i]);
	  }

	  vxReleaseGraph(&graph);
	}

	for (int i = 0; i < 5; i++) {
	  vxReleaseImage(&images[i]);
	}
	vxReleaseContext(&context);
  }
}
