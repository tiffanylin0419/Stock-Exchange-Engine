#include "connect_func.h"
#include "parse.hpp"


class Server {
 private:
  const char * port_num;
  

 public:
  static int connected;
  Server(const char * myport) : port_num(myport) {}
  void run();
  static void * handle(void * info);
};