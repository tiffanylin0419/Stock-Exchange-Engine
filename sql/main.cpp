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
  add_account(C, 11, 30);
  add_account(C, 12, 34);
  add_stock(C, 11, "ab", 3);
  add_stock(C, 11, "BTS", 7);
  add_stock(C, 12, "BIT", 9);
  add_order(C, 11, "stock1", 3, 3, "open");
  add_order(C, 12, "BTS", 3, 101, "open");
  add_order(C, 11, "BTS", 4, -100, "open");


  C->disconnect();
  return 0;
}

