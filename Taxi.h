#ifndef _TAXI_H_
#define _TAXI_H_

#include <vector>
#include <string>

namespace paras {
class Taxi {
public:
  // Pickup schedule
  std::vector<long> schedule;

  // Capacity of the taxi
  int capacity;

  // Current position of the taxi
  double cur_x;
  double cur_y;

  int serialize(std::string &msg);

  static Taxi deserialize(std::string msg);
};
}

#endif
