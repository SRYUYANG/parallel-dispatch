#ifndef _ASSIGNMENT_H_
#define _ASSIGNMENT_H_

#include <unordered_map>
#include "Taxi.h"
#include "Passenger.h"

namespace paras {
class Assignment {
public:
  std::vector<Taxi> schedule;
  std::unordered_map<long, Passenger> passenger_list;

  //Cost of the assignment
  double cost;
};
}

#endif
