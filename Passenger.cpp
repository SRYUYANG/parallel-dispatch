#include "Passenger.h"

#include <vector>

#include "Util.h"

namespace paras {
int Passenger::serialize(std::string &msg) {
  msg += std::to_string(id) + ',' + std::to_string(start_x) + ',' +
         std::to_string(start_y) + ',' + std::to_string(dest_x) + ',' +
         std::to_string(dest_y) + ',' + std::to_string(num_passenger);

  return msg.size() + 1;
}

Passenger Passenger::deserialize(std::string msg) {
  Passenger tmp;
  std::vector<std::string> tokens;
  Util::split(msg, ',', tokens);
  tmp.id = std::stol(tokens[0]);
  tmp.start_x = std::stod(tokens[1]);
  tmp.start_y = std::stod(tokens[2]);
  tmp.dest_x = std::stod(tokens[3]);
  tmp.dest_y = std::stod(tokens[4]);

  tmp.num_passenger = std::stoi(tokens[5]);

  return tmp;
}
}
