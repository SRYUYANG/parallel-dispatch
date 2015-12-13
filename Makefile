all: main.cpp
	mpic++ -std=c++11 main.cpp Passenger.cpp Taxi.cpp Assignment.cpp Util.cpp -o main -O3 
test: lsap-test.cpp Hungarian.cpp
	clang++ -std=c++11 -Wall lsap-test.cpp Hungarian.cpp -o test
