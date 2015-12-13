#include <iostream>
#include <string>
#include "Assignment.h"

using namespace std;
using namespace paras;

int main() {
  Assignment ass("small_test.txt");
  std::string msg;
  ass.serialize(msg);

  cout << msg << endl;

  Assignment* ass2 = Assignment::deserialize(msg);

  std::string sec_msg;

  ass2->serialize(sec_msg);

  cout << sec_msg << endl;

  delete ass2;

}
