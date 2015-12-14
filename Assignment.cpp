#include "Assignment.h"
#include "Util.h"
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <unordered_map>

namespace paras {

/**
 * Constructor
 */
Assignment::Assignment(std::string file_name) {
  current_cost = std::numeric_limits<double>::max();
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

  for (int i = 0; i < num_passenger; i++) {
    Passenger pass;
    infile >> pass.id >> pass.start_x >> pass.start_y >> pass.dest_x >>
        pass.dest_y >> pass.num_passenger;
    passenger_list[pass.id] = pass;
  }
  infile.close();
}

/**
 * Serialization
 * @param  msg [description]
 * @return     [description]
 */
int Assignment::serialize(std::string &msg) {
  scheduleSerialize(msg);
  passengerSerialize(msg);
  return msg.size() + 1;
}

/**
 * Schedule serialization
 * @param  msg [description]
 * @return     [description]
 */
int Assignment::scheduleSerialize(std::string &msg) {
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
  return msg.size() + 1;
}

/**
 * Deserialize schedule
 */
std::vector<Taxi> *Assignment::scheduleDeserialize(std::string msg) {
  std::vector<Taxi> *tmp = new std::vector<Taxi>();
  std::vector<std::string> tokens;
  Util::split(msg, '$', tokens);
  int schedule_size = std::stoi(tokens[0]);
  std::vector<std::string> schedule_tokens;
  Util::split(tokens[1], ';', schedule_tokens);
  for (int i = 0; i < schedule_size; i++) {
    tmp->push_back(Taxi::deserialize(schedule_tokens[i]));
  }
  return tmp;
}

/**
 * Deserialize Assignment
 * @param  msg [description]
 * @return     [description]
 */
Assignment *Assignment::deserialize(std::string msg) {
  Assignment *assignment = new Assignment();
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

/**
 * Serialize Passenger
 * @param  msg [description]
 * @return     [description]
 */
int Assignment::passengerSerialize(std::string &msg) {
  int list_size = passenger_list.size();
  msg += std::to_string(list_size) + '$';

  if (list_size != 0) {
    for (auto &item : passenger_list) {
      msg += std::to_string(item.first) + '#';
      item.second.serialize(msg);
      msg += '#';
    }
  }
  msg += "$";

  return msg.size() + 1;
}

/**
 * Deserialize Passenger;
 */
std::unordered_map<long, Passenger> *passengerDeserialize(std::string msg) {
  std::vector<std::string> tokens;
  Util::split(msg, '$', tokens);
  std::unordered_map<long, Passenger> *passengers =
      new std::unordered_map<long, Passenger>;
  int passenger_list_size = std::stoi(tokens[0]);
  std::vector<std::string> passenger_list_tokens;
  Util::split(tokens[1], '#', passenger_list_tokens);
  for (int i = 0; i < passenger_list_size * 2; i += 2) {
    (*passengers)[std::stol(passenger_list_tokens[i])] =
        Passenger::deserialize(passenger_list_tokens[i + 1]);
  }
  return passengers;
}

void Assignment::initAssignment() {
  // Initialize empty schedule
  std::vector<Taxi> new_schedule = schedule;

  // Insert passenger in one by one;
  for (auto &passenger : passenger_list) {
    // Iterate through the taxis to find place
    double minimum_cost = std::numeric_limits<double>::max();
    int minimum_index = 0;
    std::vector<long> best_vec = new_schedule[0].schedule;

    //Search for all possible insertion position
    for (int i = 0; i < new_schedule.size(); i++) {
      Taxi &taxi = new_schedule[i];

      if (taxi.capacity < passenger.second.num_passenger)
        continue;

      std::vector<long> old_vec = taxi.schedule;
      std::vector<long> new_vec;

      for (int j = 0; j <= old_vec.size(); j++) {
        new_vec = old_vec;
        new_vec.insert(new_vec.begin() + j, passenger.first);
        std::vector<long> new_inner_vec;

        for (int k = j + 1; k <= new_vec.size(); k++) {
          new_inner_vec = new_vec;
          new_inner_vec.insert(new_inner_vec.begin() + k, -passenger.first);
          taxi.schedule = new_inner_vec;
          double cost = Assignment::calculateCost(new_schedule, passenger_list);
          if (cost < minimum_cost) {
            best_vec = new_inner_vec;
            minimum_index = i;
            minimum_cost = cost;
          }
        }

      }

      taxi.schedule = old_vec;
    }

    new_schedule[minimum_index].schedule = best_vec;
    new_schedule[minimum_index].capacity -= passenger.second.num_passenger;
  }

  schedule = new_schedule;

  std::cout << "@@@ Initial Cost after insertion: " << getCost() << std::endl;
  printSchedule();
}

double
Assignment::calculateCost(std::vector<Taxi> &temp_schedule,
                          std::unordered_map<long, Passenger> &pass_list) {
  double total_cost = 0;
  int count = 0;
  for (auto &item : temp_schedule) {
    count += item.schedule.size() / 2;
    if (item.schedule.size() > 0) {
      double x_now = item.cur_x;
      double y_now = item.cur_y;
      std::unordered_map<long, double> travel_cost;
      double wait_cost = 0;
      travel_cost[item.schedule[0]] = 0;

      double x_start = pass_list[item.schedule[0]].start_x;
      double y_start = pass_list[item.schedule[0]].start_y;

      wait_cost += Util::getCost(x_now, y_now, x_start, y_start);
      total_cost += wait_cost;

      x_now = x_start;
      y_now = y_start;

      for (int i = 1; i < item.schedule.size(); i++) {
        long id = item.schedule[i];

        if (id > 0) {
          x_start = pass_list[id].start_x;
          y_start = pass_list[id].start_y;
          double cost = Util::getCost(x_now, y_now, x_start, y_start);
          wait_cost += cost;
          total_cost += wait_cost;
          for (auto &picked_up : travel_cost) {
            if (picked_up.second >= 0) {
              picked_up.second += cost;
            }
          }
          travel_cost[id] = 0;
          x_now = x_start;
          y_now = y_start;
        } else {
          id = -id;
          double x_end = pass_list[id].dest_x;
          double y_end = pass_list[id].dest_y;
          double cost = Util::getCost(x_now, y_now, x_end, y_end);
          wait_cost += cost;
          for (auto &picked_up : travel_cost) {
            if (picked_up.second >= 0) {
              picked_up.second += cost;
            }
          }

          total_cost += travel_cost[id];
          travel_cost[id] = -1;

          x_now = x_end;
          y_now = y_end;
        }
      }
    }
  }

  //Add 100000 cost to the system.
  total_cost += 100000 * (pass_list.size() - count);

  return total_cost;
}

double Assignment::getCost() {
  return Assignment::calculateCost(schedule, passenger_list);
}

void Assignment::printSchedule() {
  for (int i = 0; i < schedule.size(); i++) {
    std::cout << "Taxi: " << i << " Schedule: ";
    for (int j = 0; j < schedule[i].schedule.size(); j++) {
      std::cout << schedule[i].schedule[j] << " " ;
    }
    std::cout << std::endl;
  }
}
}
