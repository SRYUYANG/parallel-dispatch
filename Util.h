#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>
#include <sstream>
#include <vector>

namespace paras {
class Util {
public:
  static void split(const std::string &s, char delim,
                    std::vector<std::string> &elems);
  static double getCost(double& x_1, double& y_1, double& x_2, double& y_2);
};
}
#endif
