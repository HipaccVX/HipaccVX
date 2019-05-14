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


#define WIDTH  2048 
#define HEIGHT 2048 
#define IMAGE  "../../common/img/nature.jpg"


using namespace hipacc;
using namespace hipacc::math;


// Gaussian blur filter in Hipacc
class GaussianBlur : public Kernel<uchar> {
    private:
        Accessor<uchar> &input;
        Mask<float> &mask;

    public:
        GaussianBlur(IterationSpace<uchar> &iter, Accessor<uchar> &input,
                     Mask<float> &mask)
              : Kernel(iter), input(input), mask(mask) {
            add_accessor(&input);
        }

        void kernel() {
            output() = (uchar)(convolve(mask, Reduce::SUM, [&] () -> float {
                    return mask() * input(mask);
                }) + 0.5f);
        }
};


// forward declaration of reference implementation
void gaussian_filter(uchar *in, uchar *out, float *filter,
                     int size_x, int size_y, int width, int height);


/*************************************************************************
 * Main function                                                         *
 *************************************************************************/
int main(int argc, const char **argv) {
    const int width = WIDTH;
    const int height = HEIGHT;

    // convolution filter mask
    const float coef[5][5] = {
			{0.0032625f, 0.0142519f, 0.0220895f, 0.0142519f, 0.0032625f},
			{0.0142519f, 0.0622580f, 0.0964958f, 0.0622580f, 0.0142519f},
			{0.0220895f, 0.0964958f, 0.1495622f, 0.0964958f, 0.0220895f},
			{0.0142519f, 0.0622580f, 0.0964958f, 0.0622580f, 0.0142519f},
			{0.0032625f, 0.0142519f, 0.0220895f, 0.0142519f, 0.0032625f}
    };

    // host memory for image of width x height pixels
    uchar *input = load_data<uchar>(width, height, 1, IMAGE);
    uchar *ref_out = new uchar[width*height];

    std::cout << "Calculating Hipacc Gaussian filter ..." << std::endl;

    //************************************************************************//

    // input and output image of width x height pixels
    Image<uchar> in(width, height, input);
    Image<uchar> out(width, height);

    // define Mask for Gaussian filter
    Mask<float> mask(coef);
    BoundaryCondition<uchar> bound(in, mask, Boundary::UNDEFINED);
    Accessor<uchar> acc(bound);
    IterationSpace<uchar> iter(out);

    GaussianBlur filter(iter, acc, mask);

    for(int i=0; i<50; i++){
      filter.execute();
      // get pointer to result data
      uchar *output = out.data();
    }

    //************************************************************************//
    // get pointer to result data
    uchar *output = out.data();

    save_data(width, height, 1, input, "input.jpg");
    save_data(width, height, 1, output, "output.jpg");
    //show_data(width, height, 1, output, "output.jpg");

    // free memory
    delete[] input;
    delete[] ref_out;

    return EXIT_SUCCESS;
}


// Gaussian blur filter reference
void gaussian_filter(uchar *in, uchar *out, float *filter,
                     int size_x, int size_y, int width, int height) {
    int anchor_x = size_x >> 1;
    int anchor_y = size_y >> 1;
    int upper_x = width  - anchor_x;
    int upper_y = height - anchor_y;

    for (int y=anchor_y; y<upper_y; ++y) {
        for (int x=anchor_x; x<upper_x; ++x) {
            float sum = 0.5f;

            for (int yf = -anchor_y; yf<=anchor_y; ++yf) {
                for (int xf = -anchor_x; xf<=anchor_x; ++xf) {
                    sum += filter[(yf+anchor_y)*size_x + xf+anchor_x]
                            * in[(y+yf)*width + x + xf];
                }
            }
            out[y*width + x] = (uchar) (sum);
        }
    }
}
