#include <iostream>
#include <mpi.h>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include "Util.h"
#include "Assignment.h"

using namespace paras;

void coordinator() {

  int com_size;

  MPI_Comm_size(MPI_COMM_WORLD, &com_size);

  paras::Assignment assignment("small_test.txt");

  std::string msg;

  int msg_size = assignment.serialize(msg);

  std::cout << msg << std::endl;

  char *buff = new char[msg_size];
  strcpy(buff, msg.c_str());
  MPI_Bcast(&msg_size, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);
  MPI_Bcast(buff, msg_size, MPI_CHAR, 0, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  delete[] buff;
}

void worker(int rank) {
  int msg_size;
  MPI_Bcast(&msg_size, 1, MPI_INTEGER, 0,MPI_COMM_WORLD);

  char *buff = new char[msg_size];
  MPI_Bcast(buff, msg_size, MPI_CHAR, 0, MPI_COMM_WORLD);

  paras::Assignment *ass = paras::Assignment::deserialize(std::string(buff));

  std::cout << buff << std::endl;

  delete[] buff;
  delete ass;

  MPI_Barrier(MPI_COMM_WORLD);
}

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    coordinator();
  } else {
    worker(rank);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Finalize();
}
