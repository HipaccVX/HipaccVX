

SOURCE=main.cpp

INCLUDE_DIR="-I ./VX"

CC=g++

all: main

main: main.cpp VX/hipaVX/hipaVX.hpp VX/hipaVX/hipaVX_hipacc_generator.hpp
	$(CC) $(INCLUDE_DIR) main.cpp -o main

clean:
	rm -f main
