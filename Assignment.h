#ifndef _ASSIGNMENT_H_
#define _ASSIGNMENT_H_

#include <map>
#include <unordered_map>
#include <string>
#include "Taxi.h"
#include "Passenger.h"
#include <limits>

namespace paras {
class Assignment {
public:
  std::vector<Taxi> schedule;
  std::unordered_map<long, Passenger> passenger_list;

  double current_cost;

  int serialize(std::string &msg);
  static Assignment *deserialize(std::string msg);

  int scheduleSerialize(std::string &msg);
  static std::vector<Taxi> *scheduleDeserialize(std::string msg);

  int passengerSerialize(std::string &msg);
  static std::unordered_map<long, Passenger> *
  passengerDeserialize(std::string msg);

  Assignment() : current_cost(std::numeric_limits<double>::max()) {}
  Assignment(std::string file_name);

  void initAssignment();
  static double calculateCost(std::vector<Taxi> &temp_schedule,
                              std::unordered_map<long, Passenger> &pass_list);

  double getCost();
};
}

#endif
