#include <iostream>
#include <mpi.h>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include "Util.h"
#include "Assignment.h"
#include "Protocal.h"

namespace paras {
void coordinator(std::string file_name) {

  double start_time = MPI_Wtime();

  // Read Passenger and Taxi Schedule from file;
  paras::Assignment assignment(file_name);

  // Initialize start assignment;
  assignment.initAssignment();

  // Broadcast initial assignment to all the workers.
  std::string msg;
  int msg_size = assignment.serialize(msg);
  char *buff = new char[msg_size];
  strcpy(buff, msg.c_str());
  MPI_Bcast(&msg_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(buff, msg_size, MPI_CHAR, 0, MPI_COMM_WORLD);

  int q_count = 0;
  int q_limit = assignment.schedule.size();

  int idle_count;
  MPI_Comm_size(MPI_COMM_WORLD, &idle_count);

  idle_count--;

  Assignment best_assignment = assignment;
  double best_cost = best_assignment.getCost();

  while (true) {
    if (idle_count == 0) {
      break;
    }
    MPI_Status mpi_status;
    int garbage = 0;
    MPI_Recv(&garbage, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,
             &mpi_status);
    if (mpi_status.MPI_TAG == REQUIRE_JOB) {
      if (q_count < q_limit) {
        MPI_Request mpi_request;
        MPI_Isend(&q_count, 1, MPI_INT, mpi_status.MPI_SOURCE, REQUIRE_JOB,
                  MPI_COMM_WORLD, &mpi_request);
        q_count++;
      } else {
        MPI_Request mpi_request;
        MPI_Isend(&q_count, 1, MPI_INT, mpi_status.MPI_SOURCE, EXIT,
                  MPI_COMM_WORLD, &mpi_request);
        idle_count--;
      }
    }

    if (mpi_status.MPI_TAG == REPORT) {
      double cost = 0;
      MPI_Recv(&cost, 1, MPI_DOUBLE, mpi_status.MPI_SOURCE, REPORT,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      std::cout << "### Received proc " << mpi_status.MPI_SOURCE
                << " report. Cost: " << cost << " best_cost: " << best_cost
                << std::endl;

      if (cost < best_cost) {
        int allowed = 1;
        MPI_Send(&allowed, 1, MPI_INT, mpi_status.MPI_SOURCE, REPORT,
                 MPI_COMM_WORLD);

        int msg_size = 0;
        MPI_Recv(&msg_size, 1, MPI_INT, mpi_status.MPI_SOURCE, REPORT,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        char *msg_buff = new char[msg_size];
        MPI_Recv(msg_buff, msg_size, MPI_CHAR, mpi_status.MPI_SOURCE, REPORT,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        Assignment *new_ass = Assignment::deserialize(std::string(msg_buff));

        best_assignment = *new_ass;
        best_cost = cost;

        std::cout << "### Update complete" << std::endl;

        delete new_ass;
      } else {
        std::cout << "### Request rejected" << std::endl;
        int allowed = 0;
        MPI_Send(&allowed, 1, MPI_INT, mpi_status.MPI_SOURCE, REPORT,
                 MPI_COMM_WORLD);
      }
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  double end_time = MPI_Wtime();
  std::cout << "\n\n======================Report=====================\n";
  std::cout << "### Running time: " << end_time - start_time << std::endl;
  std::cout << "### Final result cost: " << best_cost << std::endl;
  std::cout << "### Final schedule: " << std::endl;
  best_assignment.printSchedule();
  std::cout << "\n========================End======================="
            << std::endl;

  delete[] buff;
}
}
