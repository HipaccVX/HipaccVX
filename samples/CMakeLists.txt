include_directories(${ROOT_DIR})

add_subdirectory(gauss_sobel_mag_thresh)
add_subdirectory(teaser)
add_subdirectory(tomasi)

add_subdirectory(vxAndNode)
add_subdirectory(vxOrNode)
add_subdirectory(vxXorNode)
add_subdirectory(vxAbsDiffNode)
add_subdirectory(vxErode3x3Node)
add_subdirectory(vxDilate3x3Node)
add_subdirectory(vxBox3x3Node)
add_subdirectory(vxGaussian3x3Node)
add_subdirectory(vxNotNode)
add_subdirectory(vxEqualizeHistNode)
add_subdirectory(vxChannelExtractNode)
add_subdirectory(vxChannelCombineNode)
add_subdirectory(vxCopyNode)
add_subdirectory(vxSobel3x3Node)
add_subdirectory(vxMagnitudeNode)
add_subdirectory(vxMultiplyNode)
add_subdirectory(vxThresholdNode)
add_subdirectory(vxConvolveNode)
add_subdirectory(vxNonMaxSuppressionNode)
add_subdirectory(vxAddNode)
add_subdirectory(vxSubtractNode)
add_subdirectory(vxConvertDepthNode)
add_subdirectory(vxHarrisCornersNode)
add_subdirectory(vxMedian3x3Node)
add_subdirectory(vxFastCornersNode)
add_subdirectory(vxIntegralImageNode)
add_subdirectory(vxMeanStdDevNode)

# get_directory_property(targets BUILDSYSTEM_TARGETS)
# foreach(TARGET_DEFINED ${targets})
# 	get_target_property(tmp ${TARGET_DEFINED} NAME)
# 	create_hipacc_version(${tmp})
# endforeach()
