#include <iostream>
#include <pqxx/pqxx>

#include "query_funcs.h"

using namespace std;
using namespace pqxx;

int main (int argc, char *argv[]) 
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
  //TODO: create PLAYER, TEAM, STATE, and COLOR tables in the ACC_BBALL database
  //      load each table with rows from the provided source txt files
  deleteTable(C, "ACCOUNT");
  deleteTable(C, "STOCK");
  deleteTable(C, "ORDERS");
  createTable("account.sql", C);
  createTable("stock.sql", C);
  createTable("order.sql", C);
  /*insertAccount("account.txt", C);
  insertStock("stock.txt", C);
  insertOrder("order.txt", C);*/

  // add account
  cout << add_account(C, 1, 40);
  cout << add_account(C, 2, 304);
  cout << add_account(C, 3, 30400);
  cout << add_account(C, 4, 30400);
  // add account fail
  cout << add_account(C, 1, 32);
  //add stock
  cout << add_stock(C, 1, "a", 3);
  cout << add_stock(C, 1, "b", 7);
  cout << add_stock(C, 2, "c", 9);
  cout << add_stock(C, 3, "c", 9);
  //add stock fail
  cout << add_stock(C, -1, "a", 3);
  //add order
  cout << add_order(C, 1, "d", 3, 3);
  cout << add_order(C, 1, "b", 3, -101);
  cout << add_order(C, 2, "b", 3, 101);
  //add order no account
  cout << add_order(C, -1, "d", 3, -3);
  //add order no stock
  cout << add_order(C, 1, "d", 3, -3);
  //add order no money
  cout << add_order(C, 2, "b", 8, 101);
  //check buy exact num
  cout << add_stock(C, 1, "e", 19);
  cout << add_order(C, 1, "e", 4, -100);
  cout << add_order(C, 4, "b", 4, 100);
  cout << add_order(C, 3, "e", 4, 101);
  //check buy all, sell left
  cout << add_stock(C, 1, "f", 19);
  cout << add_order(C, 1, "f", 4, -100);
  cout << add_order(C, 3, "f", 3, 101);


  C->disconnect();
  return 0;
}

