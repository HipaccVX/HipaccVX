export CPLUS_INCLUDE_PATH=../../
set -x
g++ -std=c++11 -Wall  07* ../../hipaVX/dsl/*.cpp ../../hipaVX/graph/*.cpp
./a.out
