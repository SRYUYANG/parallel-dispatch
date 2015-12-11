all: main.cpp
	mpic++ -std=c++11 main.cpp -o main
test: lsap-test.cpp Hungarian.cpp
	clang++ -std=c++11 -Wall lsap-test.cpp Hungarian.cpp -o test
