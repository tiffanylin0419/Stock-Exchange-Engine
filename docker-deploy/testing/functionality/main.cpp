//#include "head.h"
#include "connect_func.h"

using namespace std;

int BUFFER_MAX_LEN=128;

string read_file_to_string(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file");
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


int main(int argc, char * argv[]) {
  //input
  /*if (argc != 3) {
    cerr << "player <host name> <port num>" << endl;
    exit(EXIT_FAILURE);
  }*/
  
  //connect to server
  //int server_fd = setup_client(argv[1], argv[2]);

  int server_fd = setup_client("localhost", "12345");

  string request=read_file_to_string(argv[1]);
  int l=request.length();
  //send(server_fd, &l, sizeof(l), 0);
  send(server_fd, request.c_str(), 6500, 0);

  char buffer[6500];
  memset(buffer, 0, sizeof(buffer));//clear buffer
  //recv(server_fd, &l,  sizeof(l), 0);
  recv(server_fd, buffer,  6500, 0);
  cout<<buffer<<endl;
  exit(EXIT_SUCCESS);
}