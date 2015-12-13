#include <iostream>
#include <mpi.h>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include "Util.h"
#include "Assignment.h"

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
  MPI_Bcast(&msg_size, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);
  MPI_Bcast(buff, msg_size, MPI_CHAR, 0, MPI_COMM_WORLD);
  
  MPI_Barrier(MPI_COMM_WORLD);
  delete[] buff;
}
}
