# HipaccVX
A transpiler for generating DSL code from OpenVX


## Current workflow
Be sure to set the correct HIPACC Pathvariable in hipacc_compile_pass/Makefile

Write the OpenVX code in main.cpp, then:

```mkdir build && cd build && cmake .. && make && ./main && cp main.hipaVX.cpp ../ && cd ..```

This creates the transpiled hipaVX.cpp file and copies it into the root of the repository

```cd hipacc_compile_pass && make```

In- and Outputimages are in the hipacc_compile_pass folder
