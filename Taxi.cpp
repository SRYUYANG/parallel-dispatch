#include "Taxi.h"
#include "Util.h"

namespace paras {
int Taxi::serialize(std::string &msg) {
  msg += std::to_string(capacity) + '*' + std::to_string(cur_x) + '*' +
         std::to_string(cur_y) + '*';
  msg += std::to_string(schedule.size()) + '*';
  for (long item : schedule) {
    msg += std::to_string(item) + '@';
  }

  msg += '*';

  return msg.size() + 1;
}

Taxi Taxi::deserialize(std::string msg) {
  std::vector<std::string> tokens;
  Util::split(msg, '*', tokens);

  Taxi tmp;
  tmp.capacity = std::stoi(tokens[0]);
  tmp.cur_x = std::stod(tokens[1]);
  tmp.cur_y = std::stod(tokens[2]);

  int schedule_size = std::stod(tokens[3]);
  std::vector<std::string> schedule_tokens;
  Util::split(tokens[4], '@', schedule_tokens);

  for (int i = 0; i < schedule_size; i++) {
    tmp.schedule.push_back(std::stol(schedule_tokens[i]));
  }

  return tmp;
}
}
