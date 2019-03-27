export CPLUS_INCLUDE_PATH=../../
set -x
g++ -Wall -Werror 01* ../../hipaVX/domVX_types.cpp
./a.out
