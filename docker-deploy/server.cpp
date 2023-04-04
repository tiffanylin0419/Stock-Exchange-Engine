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
    //pthread_create(&thread, NULL, handle, clientdata);

    std :: string request;
    int l;
    //char buffer[1024];
    //memset(buffer, 0, sizeof(buffer));//clear buffer
    //recv(client_fd, &l,  sizeof(l), 0);
    //recv(client_fd, buffer,  l, 0);
    request = receive(client_fd, l);
    //cout<<"\nrequestTest\n"<<request<<"\nrequestTest\n";
    //cout<<requestToResponse(C, request)<<endl<<endl;
    string response=requestToResponse(C, request);
    l=response.length();
    send(client_fd, &l, sizeof(l), 0);
    send(client_fd, response.c_str(), l, 0);

  }
  close(temp_fd);
}

/*
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
    //pthread_create(&thread, NULL, handle, clientdata);

    int l;
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));//clear buffer
    recv(client_fd, &l,  sizeof(l), 0);
    recv(client_fd, buffer,  l, 0);
    //cout<<"\nrequestTest\n"<<buffer<<"\nrequestTest\n";
    //cout<<requestToResponse(C, buffer)<<endl<<endl;
    string response=requestToResponse(C, buffer);
    l=response.length();
    send(client_fd, &l, sizeof(l), 0);
    send(client_fd, response.c_str(), l, 0);

  }
  close(temp_fd);
}
*/
