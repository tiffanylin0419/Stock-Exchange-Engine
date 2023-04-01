#include "server.h"
#include "client_data.h"
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void Server::run() {
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
    //pthread_create(&thread, NULL, handle, clientdata);

    char buffer[512];
    recv(client_fd, buffer, 9, 0);
    buffer[9] = 0;

    cout << "Server received: " << buffer << endl;
    
  }
  close(temp_fd);
}