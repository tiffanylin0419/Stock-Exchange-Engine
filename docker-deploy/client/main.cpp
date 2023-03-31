//#include "head.h"
#include "connect_func.h"

using namespace std;

int BUFFER_MAX_LEN=128;

int main(int argc, char * argv[]) {
  //input
  /*if (argc != 3) {
    cerr << "player <host name> <port num>" << endl;
    exit(EXIT_FAILURE);
  }*/
  
  //connect to server
  //int server_fd = setup_client(argv[1], argv[2]);
  int server_fd = setup_client("127.0.0.1", "12345");

  const char *message = "hi there!";
  send(server_fd, message, strlen(message), 0);

  exit(EXIT_SUCCESS);
}