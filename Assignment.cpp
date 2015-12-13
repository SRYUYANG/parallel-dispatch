#include "Assignment.h"
#include "Util.h"
#include <fstream>
#include <iostream>

namespace paras {
int Assignment::serialize(std::string &msg) {
  int schedule_size = schedule.size();
  msg += std::to_string(schedule_size) + '$';
  if (schedule_size != 0) {
    for (int i = 0; i < schedule_size - 1; i++) {
      schedule[i].serialize(msg);
      msg += ';';
    }
    schedule[schedule_size - 1].serialize(msg);
  }
  msg += '$';

  int list_size = passenger_list.size();
  msg += std::to_string(list_size) + '$';

  if (list_size != 0) {
    for (auto &item : passenger_list) {
      msg += std::to_string(item.first) + '#';
      item.second.serialize(msg);
      msg += '#';
    }
  }

  return msg.size() + 1;
}

Assignment* Assignment::deserialize(std::string msg) {
  Assignment* assignment = new Assignment();
  std::vector<std::string> tokens;

  Util::split(msg, '$', tokens);

  int schedule_size = std::stoi(tokens[0]);

  std::vector<std::string> schedule_tokens;

  Util::split(tokens[1], ';', schedule_tokens);

  for (int i = 0; i < schedule_size; i++) {
    assignment->schedule.push_back(Taxi::deserialize(schedule_tokens[i]));
  }

  schedule_tokens.clear();

  int passenger_list_size = std::stoi(tokens[2]);

  std::vector<std::string> passenger_list_tokens;

  Util::split(tokens[3], '#', passenger_list_tokens);

  for (int i = 0; i < passenger_list_size * 2; i += 2) {
    assignment->passenger_list[std::stol(passenger_list_tokens[i])] =
        Passenger::deserialize(passenger_list_tokens[i + 1]);
  }

  passenger_list_tokens.clear();

  tokens.clear();

  return assignment;
}

Assignment::Assignment(std::string file_name) {
  std::ifstream infile(file_name);
  int num_taxi;
  infile >> num_taxi;

  for (int i = 0; i < num_taxi; i++) {
    Taxi taxi;
    infile >> taxi.cur_x >> taxi.cur_y >> taxi.capacity;
    schedule.push_back(taxi);
  }



  int num_passenger;
  infile >> num_passenger;

  std::cout << num_passenger << std::endl;

  for (int i = 0; i < num_passenger; i++) {
    Passenger pass;
    infile >> pass.id >> pass.start_x >> pass.start_y >> pass.dest_x >>
        pass.dest_y >> pass.num_passenger;
    passenger_list[pass.id] = pass;
  }
  infile.close();
}

Assignment::Assignment(){}
}
