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


string read_file_to_string(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file");
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}