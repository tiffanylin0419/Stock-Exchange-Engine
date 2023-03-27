
#include "tool.h"


string float_to_string(float value) {
  ostringstream ss;
  ss << fixed << setprecision(2) << value;
  return ss.str();
}

int getTime(){
  time_t now_seconds = time(nullptr);
  return now_seconds;
}