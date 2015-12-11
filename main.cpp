#include <iostream>
#include <mpi.h>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include "Util.h"

struct assignment {
  int a;
  double b;
  char c;

  int serialize(std::string &msg) {
    msg += std::to_string(a) + "$";
    msg += std::to_string(b) + "$";
    msg += c;
    return msg.size() + 1;
  }
  static assignment *deserialize(std::string in_str) {
    assignment *obj = new assignment();
    std::vector<std::string> tokens;
    paras::Util::split(in_str, '$', tokens);
    obj->a = std::stoi(tokens[0]);
    obj->b = std::stod(tokens[1]);
    obj->c = tokens[2][0];
    return obj;
  }

  void print() {
    std::string msg;
    serialize(msg);
    std::cout << msg << std::endl;
  }
};

void coordinator() {
  assignment ass;
  ass.a = 10;
  ass.b = 100.02;
  ass.c = 'a';
  std::string msg;
  int msg_size = ass.serialize(msg);
  char *buff = new char[msg_size];
  strcpy(buff, msg.c_str());
  MPI_Request test_request;
  MPI_Isend(&msg_size, 1, MPI_INTEGER, 1, 0, MPI_COMM_WORLD, &test_request);
  MPI_Isend(buff, msg_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &test_request);
}

void worker() {
  int msg_size;
  MPI_Recv(&msg_size, 1, MPI_INTEGER, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  char *buff = new char[msg_size];
  MPI_Recv(buff, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  assignment *new_assign = assignment::deserialize(std::string(buff));
  new_assign->print();
}

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    coordinator();
  } else {
    worker();
  }
  MPI_Finalize();
}
