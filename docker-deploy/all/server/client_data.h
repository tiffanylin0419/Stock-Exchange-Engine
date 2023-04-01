#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

class ClientData {

public:
  int client_id;
  int client_fd;
  std::string request;
    //perhaps save xml object?

  public:
  ClientData(int fd, int id, std::string request) :client_fd(fd), client_id(id), request(request) {}
  ~ClientData() { close(client_fd); }
  void showInfo() {
    cout << "client_fd: " << client_fd << endl;
    cout << "client_id: " << client_id << endl;
    cout << "request: " << request << endl;
  }
};