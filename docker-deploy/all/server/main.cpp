#include "server.h"
#include "query_funcs.h"

#define PORT "12345"

int main() {  
  connection *C;
  try{ 
    C = new connection("dbname=EXCHANGE_SERVER user=postgres password=passw0rd");
    if (C->is_open()) {
    } else {
      cout << "Can't open database" << endl;
      return 1;
    } 
  } catch (const exception &e){
    cerr << e.what() << endl;
    return 1;
  }

  deleteTable(C, "ACCOUNT");
  deleteTable(C, "STOCK");
  deleteTable(C, "ORDERS");
  createTable("file/account.sql", C);
  createTable("file/stock.sql", C);
  createTable("file/order.sql", C);


  const char * port = PORT;
  Server * myServer = new Server(port);
  myServer->run();
  cout<<"test";
  return 0;
}