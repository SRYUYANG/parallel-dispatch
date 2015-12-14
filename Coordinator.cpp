#include <iostream>
#include <mpi.h>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include "Util.h"
#include "Assignment.h"
#include "Protocal.h"

namespace paras{
void coordinator(std::string file_name) {

  //Read Passenger and Taxi Schedule from file;
  paras::Assignment assignment(file_name);

  //Initialize start assignment;
  assignment.initAssignment();

  //Broadcast initial assignment to all the workers.
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

  while(true) {
    if (idle_count == 0) {
      break;
    }
    MPI_Status mpi_status;
    int garbage = 0;
    MPI_Recv(&garbage, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &mpi_status);
    if (mpi_status.MPI_TAG == REQUIRE_JOB) {
      if (q_count < q_limit) {
        MPI_Request mpi_request;
        MPI_Isend(&q_count, 1, MPI_INT, mpi_status.MPI_SOURCE, REQUIRE_JOB, MPI_COMM_WORLD, &mpi_request);
        q_count++;
      } else {
        MPI_Request mpi_request;
        MPI_Isend(&q_count, 1, MPI_INT, mpi_status.MPI_SOURCE, EXIT, MPI_COMM_WORLD, &mpi_request);
        idle_count--;
      }
    }
  }





  MPI_Barrier(MPI_COMM_WORLD);
  delete[] buff;
}
}
