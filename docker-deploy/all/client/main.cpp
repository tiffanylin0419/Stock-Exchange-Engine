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
  int server_fd = setup_client("127.0.0.1", "12345");

  string request=read_file_to_string("test2.xml");
  cout<<request<<endl<<endl;
  int l=request.length();
  send(server_fd, &l, sizeof(l), 0);

  send(server_fd, request.c_str(), l, 0);

  exit(EXIT_SUCCESS);
}