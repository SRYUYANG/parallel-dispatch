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
#include "Protocal.h"

namespace paras {

void reportAssignment(Assignment *ass, double cost);

void simulatedAnnealing(Assignment *ass, int idle) {
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
  double absolute_temp = 0.000001;
  double minimum_cost = ass->getCost();
  double global_minimum = std::numeric_limits<double>::max();

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
    } while ((target_taxi == idle) ||
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

    /*

    //------------Check if there exists global_minimum
    double local_cost = ass->getCost();

    if ((count > exchange_num || count == 1) && local_cost < global_minimum) {
      int garbage = 0;
      MPI_Send(&garbage, 1, MPI_INT, 0, CHECK_GLOBAL_MINIMUM, MPI_COMM_WORLD);
      MPI_Recv(&global_minimum, 1, MPI_DOUBLE, 0, CHECK_GLOBAL_MINIMUM,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      count = 1;

      //std::cout << "@@@ Proc " << rank << " check cost: " << local_cost
      //          << " global " << global_minimum << std::endl;
    }

    if (local_cost >= global_minimum) {
      // roll back
      // std::cout << "@@@ Proc " << rank << " prune" << std::endl;
      ass->schedule[target_taxi] = taxi_removed_old;
      temperature *= cooling_rate;
      continue;
    }
    //-----------END CHECKING

    */

    int add_index = 0;
    std::vector<long> best_vec = ass->schedule[0].schedule;
    double minimum_add_cost = std::numeric_limits<double>::max();

    // Find best taxi to insert the passenger.
    for (int i = 0; i < ass->schedule.size(); i++) {
      // Limit the swap inside of the partitioned space
      if (i == idle)
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
        std::cout << "@@@ reject" << std::endl;
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

    // Cooling down
    temperature *= cooling_rate;
  }
}

void worker() {
  /**
   * Get initialize result.
   */
  int msg_size;
  MPI_Bcast(&msg_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

  char *buff = new char[msg_size];
  MPI_Bcast(buff, msg_size, MPI_CHAR, 0, MPI_COMM_WORLD);

  paras::Assignment *ass = paras::Assignment::deserialize(std::string(buff));

  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  delete[] buff;

  while (true) {

    int garbage = 0;
    MPI_Request mpi_request;
    MPI_Isend(&garbage, 1, MPI_INT, 0, REQUIRE_JOB, MPI_COMM_WORLD,
              &mpi_request);

    int buff = 0;
    MPI_Status mpi_status;
    MPI_Recv(&buff, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &mpi_status);

    if (mpi_status.MPI_TAG == REQUIRE_JOB) {
      paras::Assignment *new_ass = new paras::Assignment();
      (*new_ass) = *ass;
      std::cout << "@@@ Proc " << rank << " assigned " << buff << std::endl;

      simulatedAnnealing(new_ass, buff);
      double cost = new_ass->getCost();

      //std::cout << "@@@ Proc: " << rank << " report: " << cost << std::endl;

      reportAssignment(new_ass, cost);

      delete new_ass;
    } else if (mpi_status.MPI_TAG == EXIT) {
      std::cout << "@@@ Proc: " << rank << " exit" << std::endl;
      break;
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
}

void reportAssignment(Assignment *ass, double cost) {
  std::string out_msg;
  MPI_Request mpi_request;
  int garbage = 0;
  MPI_Send(&garbage, 1, MPI_INT, 0, REPORT, MPI_COMM_WORLD);
  MPI_Send(&cost, 1, MPI_DOUBLE, 0, REPORT, MPI_COMM_WORLD);

  int allowed = 0;
  MPI_Status mpi_status;
  MPI_Recv(&allowed, 1, MPI_INT, 0, REPORT, MPI_COMM_WORLD, &mpi_status);

  if (allowed == 0)
    return;

  int msg_size = ass->serialize(out_msg);

  char *msg_buff = new char[msg_size];

  strcpy(msg_buff, out_msg.c_str());

  MPI_Send(&msg_size, 1, MPI_INT, 0, REPORT, MPI_COMM_WORLD);
  MPI_Send(msg_buff, msg_size, MPI_CHAR, 0, REPORT, MPI_COMM_WORLD);
}
}
