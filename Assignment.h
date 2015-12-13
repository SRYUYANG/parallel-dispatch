#ifndef _ASSIGNMENT_H_
#define _ASSIGNMENT_H_

#include <unordered_map>
#include <string>
#include "Taxi.h"
#include "Passenger.h"

namespace paras {
class Assignment {
public:
  std::vector<Taxi> schedule;
  std::unordered_map<long, Passenger> passenger_list;

  int serialize(std::string &msg);
  static Assignment* deserialize(std::string msg);

  Assignment();
  Assignment(std::string file_name);
};
}

#endif
