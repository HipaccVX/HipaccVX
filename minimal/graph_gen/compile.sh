export CPLUS_INCLUDE_PATH=../../
set -x
g++ -std=c++11 -Wall -Werror  02* ../../hipaVX/domVX_types.cpp ../../hipaVX/ast.cpp
./a.out
