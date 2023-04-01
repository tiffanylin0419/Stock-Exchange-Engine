//#include "query_funcs.h"

//using namespace std;
//using namespace pqxx;

#include "server.h"

#define PORT "12345"



int main(int argc, char *argv[])
{
  connection *C;
  try{ 
    C = new connection("dbname=EXCHANGE_SERVER user=postgres password=passw0rd");
    if (C->is_open()) {
    } else {
      cout << "Can't open database" << endl;
      return 1;
    } 
  } catch (const std::exception &e){
    cerr << e.what() << std::endl;
    return 1;
  }

  deleteTable(C, "ACCOUNT");
  deleteTable(C, "STOCK");
  deleteTable(C, "ORDERS");
  createTable("file/account.sql", C);
  createTable("file/stock.sql", C);
  createTable("file/order.sql", C);

  /*string request=read_file_to_string("test1.xml");
  cout<<request<<endl<<endl;
  cout<<requestToResponse(C, request)<<endl<<endl;

  request=read_file_to_string("test2.xml");
  cout<<request<<endl<<endl;
  cout<<requestToResponse(C, request)<<endl<<endl;*/

  const char * port = PORT;
  Server * myServer = new Server(port);
  myServer->run(C);
  return 0;
}