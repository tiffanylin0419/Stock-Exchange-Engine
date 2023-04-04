#include "server.h"
#include "client_data.h"
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;


void Server::run(connection *C) {
  int temp_fd = setup_server(this->port_num);
  if (temp_fd == -1) {
    return;
  }
  int client_fd;
  int id = 0;
  while (1) {
    std::string ip;
    client_fd = accept_server(temp_fd);
    if (client_fd == -1) {
      cout << "(no-id): ERROR in connecting client" << endl;
      continue;
    }
    pthread_t thread;
    pthread_mutex_lock(&mutex1);
    ClientData * clientdata = new ClientData(client_fd, id, "");
    id++;
    pthread_mutex_unlock(&mutex1);

    std :: string request;
    int l;
    request = receive(client_fd, l);
    string response=requestToResponse(C, request);
    l=response.length();
    send(client_fd, &l, sizeof(l), 0);
    send(client_fd, response.c_str(), l, 0);
    pthread_create(&thread, NULL, handle, clientdata);
  }
  close(temp_fd);
}

void * Server::handle(void * info){
  cout<<"hi";
  return NULL;
}