#include <sstream>
#include <iomanip>

string float_to_string(float value) {
  std::ostringstream ss;
  ss << std::fixed << std::setprecision(2) << value;
  return ss.str();
}