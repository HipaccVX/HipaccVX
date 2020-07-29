export CPLUS_INCLUDE_PATH=../../
set -x
g++ -std=c++11 -Wall  03* ../../hipaVX/dsl/types.cpp ../../hipaVX/graph/*.cpp ../../hipaVX/dsl/abstractions.cpp ../../hipaVX/dsl/abstractions.cpp ../../hipaVX/gen/gen_template.cpp
./a.out
