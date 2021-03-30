# HipaccVX

An OpenVX implementation that achieves high performance for a wide variety of target platforms, namely GPUs, CPUs, and FPGAs.


## How to use

Its best to look at the examples.
Tests and sample applications are already written and can be easily compiled:

```mkdir build && cd build && cmake .. -DTARGET_PLATFORM=opencl-cpu && make -j```

This corresponding HipaCC applications uses OpenCL on the CPU and can be found in the `build/samples` or `build/tests` subdirectory.


## Publication

HipaccVX: wedding of OpenVX and DSL-based code generation
Özkan, M.A., Ok, B., Qiao, B., Teich, J., & Hannig, F. (2020). HipaccVX: Wedding of OpenVX and DSL-based Code Generation. Journal of Real-Time Image Processing. https://dx.doi.org/10.1007/s11554-020-01015-5

link: https://arxiv.org/pdf/2008.11476.pdf
