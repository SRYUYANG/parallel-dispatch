#ifndef _PASSENGER_H_
#define _PASSENGER_H_

#include <string>

namespace paras {
class Passenger {
public:
  //start coordinates
  double start_x;
  double start_y;

  //Destination coordinates
  double dest_x;
  double dest_y;

  //Num of passengers in a group
  int num_passenger;

  //Id
  long id;

  /**
   * Serialize passenger
   * @param  msg Serialized message will be append to msg
   * @return     length of the message + 1 (for NULL)
   */
  int serialize(std::string& msg);

  /**
   * Deserialization. Return an instance of Passenger.
   * @param  msg input message
   * @return     Passenger
   */
  static Passenger deserialize(std::string msg);
};
}

#endif
