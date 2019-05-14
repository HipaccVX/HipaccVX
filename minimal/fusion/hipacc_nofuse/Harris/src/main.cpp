//
// Copyright (c) 2012, University of Erlangen-Nuremberg
// Copyright (c) 2012, Siemens AG
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "hipacc.hpp"

#include <iostream>
#include <hipacc_helper.hpp>

#define SIZE_X 3
#define SIZE_Y 3
#define WIDTH 2048
#define HEIGHT 2048
#define IMAGE  "../../common/img/nature.jpg"
#define GENERATE_IMAGE

#define data_t int

using namespace hipacc;
using namespace hipacc::math;

// Harris Corner filter in Hipacc
class Sobel : public Kernel<int> {
  private:
    Accessor<int> &Input;
    Mask<int> &cMask;
    Domain &dom;

  public:
    Sobel(IterationSpace<int> &IS, Accessor<int> &Input, Mask<int> &cMask,
          Domain &dom)
          : Kernel(IS), Input(Input), cMask(cMask), dom(dom) {
        add_accessor(&Input);
    }

    void kernel() {
        int sum = 0;
        sum += reduce(dom, Reduce::SUM, [&] () -> int {
                return Input(dom) * cMask(dom);
            });
        output() = sum / 6;
    }
};

class Square1 : public Kernel<int> {
  private:
    Accessor<int> &Input;

  public:
    Square1(IterationSpace<int> &IS, Accessor<int> &Input)
          : Kernel(IS), Input(Input) {
        add_accessor(&Input);
    }

    void kernel() {
        int in = Input();
        output() = in * in;
    }
};

class Square2 : public Kernel<int> {
  private:
    Accessor<int> &Input1;
    Accessor<int> &Input2;

  public:
    Square2(IterationSpace<int> &IS, Accessor<int> &Input1,
            Accessor<int> &Input2)
          : Kernel(IS), Input1(Input1), Input2(Input2) {
        add_accessor(&Input1);
        add_accessor(&Input2);
    }

    void kernel() {
        output() = Input1() * Input2();
    }
};

class Gaussian : public Kernel<int> {
  private:
    Accessor<int> &Input;
    Mask<data_t> &cMask;
    int norm;

  public:
    Gaussian(IterationSpace<int> &IS, Accessor<int> &Input,
             Mask<data_t> &cMask, int norm)
          : Kernel(IS), Input(Input), cMask(cMask), norm(norm) {
        add_accessor(&Input);
    }

    void kernel() {
        int sum = 0;
        sum += convolve(cMask, Reduce::SUM, [&] () -> int {
                return Input(cMask) * cMask();
            });
        output() = sum / norm;
    }
};

class HarrisCorner : public Kernel<int> {
  private:
    Accessor<int> &Gx;
    Accessor<int> &Gy;
    Accessor<int> &Gxy;
    float k;
    float threshold;

  public:
    HarrisCorner(IterationSpace<int> &IS, Accessor<int> &Gx,
                 Accessor<int> &Gy, Accessor<int> &Gxy,
                 float k, float threshold)
          : Kernel(IS), Gx(Gx), Gy(Gy), Gxy(Gxy), k(k), threshold(threshold) {
        add_accessor(&Gx);
        add_accessor(&Gy);
        add_accessor(&Gxy);
    }

    void kernel() {
        int x = Gx();
        int y = Gy();
        int xy = Gxy();
        float R = 0;
        R = ((x * y) - (xy * xy))      /* det   */
            - (k * (x + y) * (x + y)); /* trace */
        int out = 0;
        if (R > threshold)
            out = 1;
        output() = out;
    }
};


/*************************************************************************
 * Main function                                                         *
 *************************************************************************/
int main(int argc, const char **argv) {
    const int width = WIDTH;
    const int height = HEIGHT;
    const int size_x = SIZE_X;
    const int size_y = SIZE_Y;
    float k = 0.04f;
    float threshold = 20000.0f;

    if (argc > 2) {
      k = atof(argv[1]);
      threshold = atof(argv[2]);
    }

    // convolution filter mask
    const int norm = 16;
    const data_t coef_xy[SIZE_Y][SIZE_X] = {
        { 1, 2, 1 },
        { 2, 4, 2 },
        { 1, 2, 1 }
    };

    const int coef_x[3][3] = {
        {-1,  0,  1},
        {-1,  0,  1},
        {-1,  0,  1}
    };

    const int coef_y[3][3] = {
        {-1, -1, -1},
        { 0,  0,  0},
        { 1,  1,  1}
    };

    // host memory for image of width x height pixels
#ifdef GENERATE_IMAGE
    int *input = new int[width*height];
    int *output = new int[width*height];
    for (int y=0; y<height; ++y) {
      for (int x=0; x<width; ++x) {
        input[y*width + x] = (y*width + x) % 199;
        output[y*width + x] = (y*width + x) % 199;
      }
    }
#else
    int *input = (int*)load_data<float>(width, height, 1, IMAGE);
    int *output = (int*)load_data<float>(width, height, 1, IMAGE);
#endif

    std::cout << "Calculating Hipacc Harris corner ..." << std::endl;

    //************************************************************************//
    // input and output image of width x height pixels
    Image<int> in(width, height, input);
    Image<int> out(width, height);
    Image<int> dx(width, height);
    Image<int> dy(width, height);
    Image<int> dxy(width, height);
    Image<int> sx(width, height);
    Image<int> sy(width, height);
    Image<int> sxy(width, height);
    Image<int> gx(width, height);
    Image<int> gy(width, height);
    Image<int> gxy(width, height);

    // define Masks for Gaussian blur and Sobel filter
    Mask<data_t> maskxy(coef_xy);
    Mask<int> maskx(coef_x);
    Mask<int> masky(coef_y);

    // define Domains for Sobel kernel
    Domain domx(maskx);
    Domain domy(masky);

    IterationSpace<int> iter_dx(dx);
    const BoundaryCondition<int> bound_in(in, maskx, Boundary::CLAMP);
    Accessor<int> acc_in(bound_in);
    Sobel derivx(iter_dx, acc_in, maskx, domx);

    IterationSpace<int> iter_dy(dy);
    Sobel derivy(iter_dy, acc_in, masky, domy);

    Accessor<int> acc_dx(dx);
    IterationSpace<int> iter_sx(sx);
    Square1 squarex(iter_sx, acc_dx);

    Accessor<int> acc_dy(dy);
    IterationSpace<int> iter_sy(sy);
    Square1 squarey(iter_sy, acc_dy);

    IterationSpace<int> iter_sxy(sxy);
    Square2 squarexy(iter_sxy, acc_dx, acc_dy);

    IterationSpace<int> iter_gx(gx);
    const BoundaryCondition<int> bound_sx(sx, maskxy, Boundary::CLAMP);
    Accessor<int> acc_sx(bound_sx);
    Gaussian gaussx(iter_gx, acc_sx, maskxy, norm);

    IterationSpace<int> iter_gy(gy);
    const BoundaryCondition<int> bound_sy(sy, maskxy, Boundary::CLAMP);
    Accessor<int> acc_sy(bound_sy);
    Gaussian gaussy(iter_gy, acc_sy, maskxy, norm);

    IterationSpace<int> iter_gxy(gxy);
    const BoundaryCondition<int> bound_sxy(sxy, maskxy, Boundary::CLAMP);
    Accessor<int> acc_sxy(bound_sxy);
    Gaussian gaussxy(iter_gxy, acc_sxy, maskxy, norm);

    IterationSpace<int> iter_out(out);
    Accessor<int> acc_gx(gx);
    Accessor<int> acc_gy(gy);
    Accessor<int> acc_gxy(gxy);
    HarrisCorner harris(iter_out, acc_gx, acc_gy, acc_gxy, k, threshold);

    for (int i = 0; i < 50; ++i) {

    float timing = 0;
    derivx.execute();
    timing += hipacc_last_kernel_timing();
    derivy.execute();
    timing += hipacc_last_kernel_timing();
    squarex.execute();
    timing += hipacc_last_kernel_timing();
    squarey.execute();
    timing += hipacc_last_kernel_timing();
    squarexy.execute();
    timing += hipacc_last_kernel_timing();
    gaussx.execute();
    timing += hipacc_last_kernel_timing();
    gaussy.execute();
    timing += hipacc_last_kernel_timing();
    gaussxy.execute();
    timing += hipacc_last_kernel_timing();
    harris.execute();
    timing += hipacc_last_kernel_timing();

    std::cout << "Hipacc overall timing: " << timing << " ms" << std::endl;
    }

    int *corners = out.data();

    //************************************************************************//

    // draw white crosses for visualization
    const int size = 5;
    for (int p = 0; p < width*height; ++p) {
        if (corners[p] != 0) {
            for (int i = -size; i <= size; ++i) {
                int posx = p+i;
                int posy = p+i*width;
                if (posx > 0 && posx < width*height) output[posx] = 255;
                if (posy > 0 && posy < width*height) output[posy] = 255;
            }
        }
    }

    save_data(width, height, 1, (float*)input, "input.jpg");
    save_data(width, height, 1, (float*)output, "output.jpg");
    //show_data(width, height, 1, (float*)output, "output.jpg");

    // free memory
    delete[] input;
    delete[] output;

    return EXIT_SUCCESS;
}
