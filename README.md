# HipaccVX

An OpenVX implementation that achieves high performance for a wide variety of target platforms, namely GPUs, CPUs, and FPGAs.


## How to use

Its best to look at the sample applications, which can be compiled for CPU, GPU, and FPGA by using CMake:

```mkdir build && cd build && cmake .. -DTARGET_PLATFORM=opencl-cpu && make -j```

Then the sample applications can be build from `build/samples` directory.


## Publication

M. Akif Özkan, Burak Ok, Bo Qiao, Jürgen Teich, and Frank Hannig

HipaccVX: Wedding of OpenVX and DSL-based code generation

Journal of Real-Time Image Processing:1–13, 2020 

DOI : 10.1007/s11554-020-01015-5

link (open access): https://link.springer.com/article/10.1007/s11554-020-01015-5

link (pdf): https://rdcu.be/chOAS
