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

class GaussianSquare2 : public Kernel<int> {
  private:
    Accessor<int> &Input1;
    Accessor<int> &Input2;
    Mask<data_t> &cMask;
    int norm;

  public:
    GaussianSquare2(IterationSpace<int> &IS, Accessor<int> &Input1,
             Accessor<int> &Input2, Mask<data_t> &cMask, int norm)
          : Kernel(IS), Input1(Input1), Input2(Input2), cMask(cMask), norm(norm) {
        add_accessor(&Input1);
        add_accessor(&Input2);
    }

    void kernel() {
        int sum = 0;
        sum += convolve(cMask, Reduce::SUM, [&] () -> int {
                int in1 = Input1(cMask);
                int in2 = Input2(cMask);
                return (in1*in2) * cMask();
            });
        output() = sum / norm;
    }
};


class GaussianSquare1 : public Kernel<int> {
  private:
    Accessor<int> &Input;
    Mask<data_t> &cMask;
    int norm;

  public:
    GaussianSquare1(IterationSpace<int> &IS, Accessor<int> &Input,
             Mask<data_t> &cMask, int norm)
          : Kernel(IS), Input(Input), cMask(cMask), norm(norm) {
        add_accessor(&Input);
    }

    void kernel() {
        int sum = 0;
        sum += convolve(cMask, Reduce::SUM, [&] () -> int {
                int in = Input(cMask);
                return (in*in) * cMask();
            });
        output() = sum / norm;
    }
};

class ShiTomasi : public Kernel<int> {
  private:
    Accessor<int> &Gx;
    Accessor<int> &Gy;
    Accessor<int> &Gxy;

  public:
    ShiTomasi(IterationSpace<int> &IS,
            Accessor<int> &Gx, Accessor<int> &Gy, Accessor<int> &Gxy)
          : Kernel(IS),
            Gx(Gx),
            Gy(Gy),
            Gxy(Gxy) {
      add_accessor(&Gx);
      add_accessor(&Gy);
      add_accessor(&Gxy);
    }

    void kernel() {
      float threshold = 200.0f;
      int x = Gx();
      int y = Gy();
      int xy = Gxy();
      float temp = 0.0;
      float lambda1 = 0.0;
      float lambda2 = 0.0;
      temp = sqrtf((x - y) * (x - y) + 4.0 * xy * xy);
      lambda1 = 0.5 * (x + y + temp);
      lambda2 = 0.5 * (x + y - temp);

      int out = 0;
      if (min(lambda1, lambda2) > threshold)
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

    std::cout << "Calculating Hipacc ShiTomasi feature ..." << std::endl;

    //************************************************************************//

    // input and output image of width x height pixels
    Image<int> in(width, height, input);
    Image<int> out(width, height);
    Image<int> dx(width, height);
    Image<int> dy(width, height);
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


    IterationSpace<int> iter_gx(gx);
    const BoundaryCondition<int> bound_dx(dx, maskxy, Boundary::CLAMP);
    Accessor<int> acc_dx(bound_dx);
    GaussianSquare1 gaussx(iter_gx, acc_dx, maskxy, norm);

    IterationSpace<int> iter_gy(gy);
    const BoundaryCondition<int> bound_dy(dy, maskxy, Boundary::CLAMP);
    Accessor<int> acc_dy(bound_dy);
    GaussianSquare1 gaussy(iter_gy, acc_dy, maskxy, norm);

    IterationSpace<int> iter_gxy(gxy);
    GaussianSquare2 gaussxy(iter_gxy, acc_dx, acc_dy, maskxy, norm);

    IterationSpace<int> iter_out(out);
    Accessor<int> acc_gx(gx);
    Accessor<int> acc_gy(gy);
    Accessor<int> acc_gxy(gxy);
    ShiTomasi shi(iter_out, acc_gx, acc_gy, acc_gxy);

    for (int i = 0; i < 50; ++i) {
    float timing = 0;
    derivx.execute();
    timing += hipacc_last_kernel_timing();
    derivy.execute();
    timing += hipacc_last_kernel_timing();
    gaussx.execute();
    timing += hipacc_last_kernel_timing();
    gaussy.execute();
    timing += hipacc_last_kernel_timing();
    gaussxy.execute();
    timing += hipacc_last_kernel_timing();
    shi.execute();
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
