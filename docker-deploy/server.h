#include "connect_func.h"
#include "parse.hpp"

class Server {
 private:
  const char * port_num;

 public:
  Server(const char * myport) : port_num(myport) {}
  void run(connection *C);
  static void * handle(void * info);
};