# HipaccVX
A transpiler for generating DSL code from OpenVX


## Current workflow
Be sure to set the correct HIPACC Pathvariable in hipacc_compile_pass/Makefile

Write the OpenVX code in main.cpp, then:

```mkdir build && cd build && cmake .. && make && cp main ../ && cd .. && ./main```

This creates the transpiled hipaVX.cpp file and copies it into the root of the repository

```cd hipacc_compile_pass && make```

In- and Outputimages are in the hipacc_compile_pass folder



## OpenVX

- [x] Absolute Difference
- [x] Accumulate
- [x] Accumulate Squared
- [x] Accumulate Weighted
- [x] Arithmetic Addition
- [x] Arithmetic Subtraction
- [x] Bitwise AND
- [x] Bitwise EXCLUSIVE OR
- [x] Bitwise INCLUSIVE OR
- [x] Bitwise NOT
- [x] Box Filter
- [ ] Canny Edge Detector
- [ ] Channel Combine
- [x] Channel Extract
- [ ] Color Convert
- [x] Convert Bit depth
- [ ] Custom Convolution
- [x] Dilate Image
- [ ] Equalize Histogram
- [x] Erode Image
- [ ] Fast Corners
- [x] Gaussian Filter
- [ ] Non Linear Filter
- [ ] Harris Corners
- [ ] Histogram
- [ ] Gaussian Image Pyramid
- [ ] Laplacian Image Pyramid
- [ ] Reconstruction from a Laplacian Image Pyramid
- [ ] Integral Image
- [x] Magnitude
- [ ] Mean and Standard Deviation
- [ ] Median Filter
- [ ] Min, Max Location
- [ ] Optical Flow Pyramid (LK)
- [x] Phase
- [x] Pixel-wise Multiplication (No rounding support)
- [ ] Remap
- [ ] Scale Image
- [x] Sobel 3x3
- [ ] TableLookup
- [x] Thresholding
- [ ] Warp Affine
- [ ] Warp Perspective




##  Add new kernel

1. Create a new class which inherits from `Node` and stores all the needed stuff. Declaration in `hipaVX_internal.hpp` and definition in `hipaVX_internal.cpp`

1. Add the forward-declaration of `std::string node_generator(..., Type t)` of your new kernel in `hipaVX_internal.hpp`

1. In order to make the next step easier and more generic for HipaCC generation a kind of templating language was used. There is a file for the kernels definition (`.def`) and instructions how to call it (`.call`).

1. In `std::string node_generator(..., Type t)` you will need to handle two cases:
    1. `generator::Type::Definition` : This is usually used for the class definitions in the generated file. The `std::string` returned from this function will get pasted outside of `main`
    1. `generator::Type::Call` : This is used to place code into the `main` function.

1. Add the declaration of the API-call in hipaVX_extension.hpp if the new kernel is an extension and not an implementation of the standard OpenVX functions

1. Implement the definition in hipaVX.cpp

