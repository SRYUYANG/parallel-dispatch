#ifndef _HUNGARIAN_H_
#define _HUNGARIAN_H_

#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <set>
#include <unordered_map>
#include <iterator>
#include <iostream>

using namespace std;

class Hungarian {
private:
  vector<vector<double>> cost_matrix;
  vector<double> dual_u_vector;
  vector<double> dual_v_vector;

  set<int> u_set;
  set<int> v_set;

  size_t size_of_matrix;

  vector<int> row_vector;
  vector<int> col_vector;

  int alternate(int k, set<int> &SU, set<int> &LV, set<int> &SV,
                unordered_map<int, int> &pred);

  void hungarian_kernel();

public:
  vector<int> solve(vector<vector<double>> cost_matrix);
  void preprocess();
};

#endif
