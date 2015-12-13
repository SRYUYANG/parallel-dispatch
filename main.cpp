#include <iostream>
#include <mpi.h>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include "Util.h"
#include "Assignment.h"
#include "Worker.cpp"
#include "Coordinator.cpp"

using namespace paras;

int main(int argc, char *argv[]) {

  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    coordinator(std::string(argv[1]));
  } else {
    worker();
  }

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Finalize();
}
