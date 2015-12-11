#include "Hungarian.h"

vector<int> Hungarian::solve(vector<vector<double>> cost_matrix) {
  size_of_matrix = cost_matrix.size();
  dual_u_vector = vector<double>(size_of_matrix, 0);
  dual_v_vector = vector<double>(size_of_matrix, 0);

  row_vector = vector<int>(size_of_matrix, -1);
  col_vector = vector<int>(size_of_matrix, -1);
  // very costy step which requires copying
  this->cost_matrix = cost_matrix;

  for (int i = 0; i < (int)size_of_matrix; i++) {
    u_set.insert(i);
    v_set.insert(i);
  }

  preprocess();

  hungarian_kernel();

  return row_vector;
}

void Hungarian::preprocess() {

  // Initialize the dual u vector
  for (size_t i = 0; i < size_of_matrix; i++) {
    double min = numeric_limits<double>::max();
    for (size_t j = 0; j < size_of_matrix; j++) {
      if (cost_matrix[i][j] < min) {
        min = dual_u_vector[i] = cost_matrix[i][j];
      }
    }
  }

  // Initialize the dual v vector
  for (size_t j = 0; j < size_of_matrix; j++) {
    double min = numeric_limits<double>::max();
    for (size_t i = 0; i < size_of_matrix; i++) {
      if (cost_matrix[i][j] - dual_u_vector[i] < min) {
        min = dual_v_vector[j] = cost_matrix[i][j] - dual_u_vector[i];
      }
    }
  }

  // Find a partial feasible solution
  for (size_t i = 0; i < size_of_matrix; i++) {
    for (size_t j = 0; j < size_of_matrix; j++) {
      if (row_vector[j] == -1 &&
          cost_matrix[i][j] - dual_u_vector[i] - dual_v_vector[j] == 0) {
        row_vector[j] = i;
        col_vector[i] = j;
        break;
      }
    }
  }

  for (auto &item : row_vector) {
    cout << item << endl;
  }
}

int Hungarian::alternate(int k, set<int> &SU, set<int> &LV, set<int> &SV,
                         unordered_map<int, int> &pred) {
  // Empty all three sets
  SU.clear();
  LV.clear();
  SV.clear();

  bool fail = false;
  int sink = -1;
  int i = k;

  while (!fail && sink == -1) {
    SU.insert(i);
    set<int> v_minus_lv;
    set_difference(v_set.begin(), v_set.end(), LV.begin(), LV.end(),
                   inserter(v_minus_lv, v_minus_lv.begin()));

    for (auto j : v_minus_lv) {
      if (cost_matrix[i][j] - dual_u_vector[i] - dual_v_vector[j] == 0) {
        pred[j] = i;
        LV.insert(j);
      }
    }

    set<int> lv_minus_sv;
    set_difference(LV.begin(), LV.end(), SV.begin(), SV.end(),
                   inserter(lv_minus_sv, lv_minus_sv.begin()));

    if (lv_minus_sv.empty()) {
      fail = true;
    } else {
      int j = *(lv_minus_sv.begin());
      SV.insert(j);
      if (row_vector[j] == -1) {
        sink = j;
      } else {
        i = row_vector[j];
      }
    }
#ifdef DEBUG
    cout << "==============================" << endl;

    cout << "i: " << i << endl;

    cout << "SU" << endl;
    for (auto &item : SU) {
      cout << item << endl;
    }

    cout << "LV" << endl;
    for (auto &item : LV) {
      cout << item << endl;
    }

    cout << "SV" << endl;
    for (auto &item : SV) {
      cout << item << endl;
    }

    cout << "sink: " << sink << endl;

    cout << "============================" << endl;
#endif
  }

  return sink;
}

void Hungarian::hungarian_kernel() {
  set<int> u_compliment;

  for (int i = 0; i < (int)size_of_matrix; i++) {
    if (row_vector[i] != -1) {
      u_compliment.insert(row_vector[i]);
    }
  }

#ifdef DEBUG
  cout << "U compliment" << endl;
  for (auto &item : u_compliment) {
    cout << item << endl;
  }
#endif

  while (u_compliment.size() < size_of_matrix) {
    set<int> u_minus_nu;

    set_difference(u_set.begin(), u_set.end(), u_compliment.begin(),
                   u_compliment.end(),
                   inserter(u_minus_nu, u_minus_nu.begin()));

    int k = *(u_minus_nu.begin());

    while (u_compliment.count(k) == 0) {
      set<int> SU;
      set<int> LV;
      set<int> SV;
      unordered_map<int, int> pred;

      int sink = alternate(k, SU, LV, SV, pred);

      if (sink >= 0) {
        u_compliment.insert(k);
        int j = sink;
        int i = -2;
        do {
          i = pred[j];
          row_vector[j] = i;
          int h = col_vector[i];
          col_vector[i] = j;
          j = h;
        } while (i != k);
      } else {
        set<int> v_minus_lv;
        set_difference(v_set.begin(), v_set.end(), LV.begin(), LV.end(),
                       inserter(v_minus_lv, v_minus_lv.begin()));

        double delta = numeric_limits<double>::max();

        for (auto i : SU) {
          for (auto j : v_minus_lv) {
            if (cost_matrix[i][j] - dual_u_vector[i] - dual_v_vector[j] <
                delta) {
              delta = cost_matrix[i][j] - dual_u_vector[i] - dual_v_vector[j];
            }
          }
        }

        for (auto i : SU) {
          dual_u_vector[i] += delta;
        }

        for (auto j : LV) {
          dual_v_vector[j] -= delta;
        }
      }
    }
  }
}
