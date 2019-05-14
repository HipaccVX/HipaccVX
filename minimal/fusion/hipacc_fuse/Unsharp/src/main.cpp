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

// kernel description in Hipacc
class GaussSharpenRatioUnsharp : public Kernel<data_t> {
  private:
    Accessor<data_t> &Input;
    Mask<data_t> &cMask;

  public:
    GaussSharpenRatioUnsharp(IterationSpace<data_t> &IS,
            Accessor<data_t> &Input, Mask<data_t> &cMask)
          : Kernel(IS),
            Input(Input),
            cMask(cMask) {
      add_accessor(&Input);
    }

    void kernel() {
      data_t reg_in = Input();
      data_t reg_gaussian, reg_sharpen, reg_ratio;

      data_t sum = 0;
      sum += convolve(cMask, Reduce::SUM, [&] () -> float {
          return Input(cMask) * cMask();
      });
      reg_gaussian = sum / 16;

      // sharpen
      reg_sharpen = 2 * reg_in - reg_gaussian; 

      // ratio
      reg_ratio = reg_sharpen / reg_in;

      // unsharp
      output() = reg_ratio * reg_in;
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
    const short norm = 16;
    const data_t coef_xy[SIZE_Y][SIZE_X] = {
        { 1, 2, 1 },
        { 2, 4, 2 },
        { 1, 2, 1 }
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

    std::cout << "Calculating Hipacc Unsharp..." << std::endl;

    //************************************************************************//

    Mask<data_t> G(coef_xy);

    Image<data_t> IN(width, height);
    Image<data_t> OUT(width, height);

    const BoundaryCondition<data_t> BcInClamp(IN, G, Boundary::CLAMP);
    Accessor<data_t> AccInClamp(BcInClamp);
    Accessor<data_t> AccIn(IN);
    IterationSpace<data_t> IS_OUT(OUT);

    GaussSharpenRatioUnsharp GaussBlur(IS_OUT, AccInClamp, G);

    // kernel invocation
    IN = input;

    for (int i = 0; i < 50; ++i) {
      float timing = 0;
      GaussBlur.execute();
      timing += hipacc_last_kernel_timing();
      std::cout << "Hipacc overall timing: " << timing << " ms" << std::endl;
    }

    output = OUT.data();

    //************************************************************************//
    save_data(width, height, 1, (float*)input, "input.jpg");
    save_data(width, height, 1, (float*)output, "output.jpg");
    //show_data(width, height, 1, (float*)output, "output.jpg");

    // free memory
    delete[] input;

    return EXIT_SUCCESS;
}
