#include "Assignment.h"
#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;
using namespace paras;

int main() {
  std::vector<Taxi> schedule;
  std::unordered_map<long, Passenger> pass_list;

  Passenger pass1;
  Passenger pass2;

  pass1.id = 1;
  pass1.start_x = 0;
  pass1.start_y = 0;
  pass1.dest_x = 3;
  pass1.dest_y = 3;

  pass2.id = 2;
  pass2.start_x = 1;
  pass2.start_y = 1;
  pass2.dest_x = 2;
  pass2.dest_y = 2;

  Taxi taxi1;
  taxi1.schedule.push_back(1);
  taxi1.schedule.push_back(2);
  taxi1.schedule.push_back(-2);
  taxi1.schedule.push_back(-1);
  taxi1.cur_x = 5;
  taxi1.cur_y = 5;

  pass_list[1] = pass1;
  pass_list[2] = pass2;

  schedule.push_back(taxi1);

  double cost = Assignment::calculateCost(schedule, pass_list);

  std::cout << cost << std::endl;
}
