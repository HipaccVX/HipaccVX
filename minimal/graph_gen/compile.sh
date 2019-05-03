export CPLUS_INCLUDE_PATH=../../
set -x
g++ -std=c++11 -Wall  03* ../../hipaVX/domVX_types.cpp ../../hipaVX/ast.cpp ../../hipaVX/abstractions.cpp
./a.out
