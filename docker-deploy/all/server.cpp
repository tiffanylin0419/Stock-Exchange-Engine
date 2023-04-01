#include "server.h"
#include "client_data.h"
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;


string requestToResponse(connection *C, string request){
  pugi::xml_document request_doc;
  // load xml parser
  pugi::xml_parse_result result = request_doc.load_string(request.c_str());
  std :: string response = "";
  if (!result || request == "") {
    // error when parsing xml
    cout << "error: parsing xml fail" << endl;
    response = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<error>Illegal "
               "XML Format</error>\n";
  }
  else if(request_doc.child("create")){
    int creat_res;
    creat_res = process_create(request_doc, response,C);
    if(creat_res == 0)
    {
      return response;
    }
    else{
      return "Create request fail.";
    }
  }
  else if(request_doc.child("transactions")){
    int trans_res;
    trans_res = process_transaction(request_doc, response,C);
    if(trans_res == 0){
      return response;
    }
    else{
      return "Transaction request fail.";
    }
  }
  else{
    response = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<error>Illegal "
              "XML Tag</error>\n";
  }
  return "wierd format XML";
}

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

    char buffer[1024];
    recv(client_fd, buffer,  sizeof(buffer), 0);
    cout<<requestToResponse(C, buffer)<<endl<<endl;
  }
  close(temp_fd);
}