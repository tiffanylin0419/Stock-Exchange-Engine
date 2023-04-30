#include "server.h"
#include "client_data.h"
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

int Server::connected = 0;

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
    pthread_create(&thread, NULL, handle, clientdata);
  }
  close(temp_fd);
}

void * Server::handle(void * info){
  ClientData * clientdata = (ClientData *)info;
  int client_fd = clientdata->client_fd;

  std :: string request;
  int l;
  request = receive(client_fd, l);

  while(Server::connected>90){
    cout<<"stuck";
  }

  pthread_mutex_lock(&mutex1);
  Server::connected++;
  pthread_mutex_unlock(&mutex1);

  connection *C;
  try{ 
    //docker
    C = new connection("dbname=postgres user=postgres password=passw0rd host=db port=5432");
    //no docker
    //C = new connection("dbname=EXCHANGE_SERVER user=postgres password=passw0rd");
    if (C->is_open()) {
    } else {
      cout << "Can't open database" << endl;
      return NULL;
    } 
  } catch (const std::exception &e){
    cerr << e.what() << std::endl;
    return NULL;
  }
  string response=requestToResponse(C, request);
  C->disconnect();
  
  pthread_mutex_lock(&mutex1);
  Server::connected--;
  pthread_mutex_unlock(&mutex1);

  l=response.length();
  //send(client_fd, &l, sizeof(l), 0);
  send(client_fd, response.c_str(), l, 0);
  std::cout<<response.c_str()<<endl;
  close(client_fd);
  return NULL;
}