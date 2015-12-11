#include <iostream>
#include <vector>

#include "Hungarian.h"

using namespace std;

int main() {

    vector< vector<double> > c_matrix{
	{7, 9, 8, 9},
	{2, 8, 5, 7},
	{1, 6, 6, 9},
	{3, 6, 2, 2}};

    Hungarian hungarian;

    vector<int> solution = hungarian.solve(c_matrix);

    for (auto &i : solution) {
	cout << i << endl;
    }
}
