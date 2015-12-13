#include "Util.h"
#include <cmath>

namespace paras {
void Util::split(const std::string &s, char delim,
                 std::vector<std::string> &elems) {

  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  ss.clear();
}

double Util::getCost(double& x_1, double& y_1, double& x_2, double& y_2) {
  return (x_1-x_2)*(x_1-x_2) + (y_1-y_2)*(y_1-y_2);
}
}
