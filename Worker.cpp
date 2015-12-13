#include <iostream>
#include <mpi.h>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include "Util.h"
#include "Assignment.h"
#include <random>
#include <algorithm>
#include <cmath>
#include <cassert>

namespace paras {
void simulatedAnnealing(Assignment *ass, int lower_bound, int upper_bound) {
  /**
   * Include ramdom generator
   */
  std::random_device rd;
  std::default_random_engine randeng(rd());

  /**
   * Initialize cooling parameters
   */
  int num_taxi = ass->schedule.size();
  double temperature = 1000;
  double cooling_rate = 0.99;
  double absolute_temp = 0.00001;
  double minimum_cost = ass->getCost();

  /**
   * Excange result every 100 iterations
   */
  int count = 0;
  int exchange_num = 100;

  int converge_count = 0;

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  while (temperature > absolute_temp) {
    count++;

    /**
     * Generate neighbor
     */
    int target_taxi = 0;

    // find the target which outside of the bounded area and
    // have non zero number of vehicles assigned.
    do {
      target_taxi = randeng() % num_taxi;
    } while ((target_taxi >= lower_bound && target_taxi < upper_bound) ||
             ass->schedule[target_taxi].schedule.empty());

    Taxi taxi_removed_old = ass->schedule[target_taxi];
    Taxi taxi_removed_new = ass->schedule[target_taxi];

    int target_pass_index = randeng() % (taxi_removed_new.schedule.size() / 2);
    int target_pass = 0;
    for (int i = 0; i < taxi_removed_new.schedule.size(); i++) {
      if (taxi_removed_new.schedule[i] > 0) {
        if (target_pass_index == 0) {
          target_pass = taxi_removed_new.schedule[i];
          break;
        }
        target_pass_index--;
      }
    }

    assert(target_pass != 0);

    Passenger &passenger = ass->passenger_list[target_pass];
    taxi_removed_new.capacity += passenger.num_passenger;

    taxi_removed_new.schedule.erase(
        std::remove(taxi_removed_new.schedule.begin(),
                    taxi_removed_new.schedule.end(), target_pass));
    taxi_removed_new.schedule.erase(
        std::remove(taxi_removed_new.schedule.begin(),
                    taxi_removed_new.schedule.end(), -target_pass));

    ass->schedule[target_taxi] = taxi_removed_new;

    int add_index = 0;
    std::vector<long> best_vec = ass->schedule[0].schedule;
    double minimum_add_cost = std::numeric_limits<double>::max();

    // Find best taxi to insert the passenger.
    for (int i = 0; i < ass->schedule.size(); i++) {
      // Limit the swap inside of the partitioned space
      if (i >= lower_bound && i < upper_bound)
        continue;

      Taxi &taxi = ass->schedule[i];

      // Not meet the capacity requirement;
      if (taxi.capacity < passenger.num_passenger)
        continue;

      // Find best place to insert the current passenger in the taxi
      std::vector<long> old_vec = taxi.schedule;
      std::vector<long> new_vec;
      for (int j = 0; j <= old_vec.size(); j++) {
        new_vec = old_vec;
        new_vec.insert(new_vec.begin() + j, target_pass);
        std::vector<long> new_inner_vec;
        for (int k = j + 1; k <= new_vec.size(); k++) {
          new_inner_vec = new_vec;
          new_inner_vec.insert(new_inner_vec.begin() + k, -target_pass);
          taxi.schedule = new_inner_vec;
          double cost = ass->getCost();
          if (cost < minimum_add_cost) {
            best_vec = new_inner_vec;
            add_index = i;
            minimum_add_cost = cost;
          }
        }
      }

      taxi.schedule = old_vec;
    }

    // Compare the updated cost and the current cost
    if (minimum_add_cost >= minimum_cost) {
      double delta_cost = minimum_add_cost - minimum_cost;

      double possibility = exp(-delta_cost / temperature);

      std::uniform_real_distribution<double> dis(0, 1);

      double roll = dis(randeng);

      if (roll > possibility) {
        // reject
        ass->schedule[target_taxi] = taxi_removed_old;
      } else {
        // accept and update the current assignment;
        ass->schedule[add_index].schedule = best_vec;
        ass->schedule[add_index].capacity -= passenger.num_passenger;
        minimum_cost = minimum_add_cost;
      }

      converge_count++;
    } else {
      // accept and update the current assignment;
      ass->schedule[add_index].schedule = best_vec;
      ass->schedule[add_index].capacity -= passenger.num_passenger;
      minimum_cost = minimum_add_cost;
      converge_count = 0;
    }

    if (converge_count == 1000) {
      break;
    }

    //Cooling down
    temperature *= cooling_rate;
  }
}

void worker() {
  /**
   * Get initialize result.
   */
  int msg_size;
  MPI_Bcast(&msg_size, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);

  char *buff = new char[msg_size];
  MPI_Bcast(buff, msg_size, MPI_CHAR, 0, MPI_COMM_WORLD);

  paras::Assignment *ass = paras::Assignment::deserialize(std::string(buff));
  delete[] buff;

  int num_taxi = ass->schedule.size();

  /**
   * Define search space which should be disjoint.
   */
  int rank, size;
  int lower_bound = -1;
  int upper_bound = -1;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Don't count coordinator.
  size = size - 1;

  // More than one processor
  if (size != 1) {
    if (rank != size) {
      lower_bound = num_taxi / size * (rank - 1);
      upper_bound = num_taxi / size * (rank);
    } else {
      lower_bound = num_taxi / size * (rank - 1);
      upper_bound = num_taxi;
    }
  }

  simulatedAnnealing(ass, lower_bound, upper_bound);

  std::cout << "@@@ Proc: " << rank << " report: " << ass->getCost() << std::endl;

  MPI_Barrier(MPI_COMM_WORLD);
}
}
