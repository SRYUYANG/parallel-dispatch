#include <iostream>
#include <fstream>
#include <random>
#include <string>

int main(int argc, char *argv[]) {
  int num_taxi = std::stoi(argv[1]);
  int num_passenger = std::stoi(argv[2]);
  std::ofstream ofile(argv[3]);
  std::random_device rd;

  std::default_random_engine randeng(rd());

  ofile << num_taxi << "\n";

  int MAP_MAX = 1000;

  for (int i = 0; i < num_taxi; i++) {
    ofile << randeng() % MAP_MAX << " " << randeng() % MAP_MAX << " "
          << randeng() % 2 + 4<< "\n";
  }

  ofile << num_passenger << "\n";

  for (int i = 0; i < num_passenger; i++) {
    ofile << i << " " << randeng() % MAP_MAX << " " << randeng() % MAP_MAX << " "
          << randeng() % MAP_MAX << " " << randeng() % MAP_MAX << " "
          << randeng() % 4 + 1 << "\n";
  }

  return 0;
}
