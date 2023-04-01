#include "query_funcs.h"

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
  createTable("file/account.sql", C);
  createTable("file/stock.sql", C);
  createTable("file/order.sql", C);
  //insertAccount("file/account.txt", C);
  //insertStock("file/stock.txt", C);
  //insertOrder("file/order.txt", C);

  // add account
  cout << add_account(C, 1, 40);
  cout << add_account(C, 2, 304);
  cout << add_account(C, 3, 30400);
  cout << add_account(C, 4, 30400);
  // add account fail
  cout << add_account(C, 1, 32);
  cout << add_account(C, 9, 0);
  cout << add_account(C, 9, -1);
  //add stock
  cout << add_stock(C, 1, "a", 3);
  cout << add_stock(C, 1, "b", 7);
  cout << add_stock(C, 2, "c", 9);
  cout << add_stock(C, 3, "c", 9);
  //add repeated stock
  cout << add_stock(C, 3, "c", 10);
  //add stock fail
  cout << add_stock(C, -1, "a", 3);
  cout << add_stock(C, 3, "c", 0);
  //add order no account
  cout << add_order(C, -1, "d", 3, -3);
  //add order no stock
  cout << add_order(C, 1, "d", 3, -3);
  cout << add_order(C, 3, "c", 20, -20);
  //add order no money
  cout << add_order(C, 2, "b", 8, 101);
  // add order amount <=0
  cout << add_order(C, 2, "b", 0, 101);
  //check buy exact num
  cout << add_stock(C, 1, "e", 19);
  cout << add_order(C, 1, "e", 4, -100);
  cout << add_order(C, 3, "e", 4, 101);
  //check buy all, sell left
  cout << add_stock(C, 1, "f", 19);
  cout << add_order(C, 1, "f", 4, -100);
  cout << add_order(C, 3, "f", 3, 101);
  //check sell all, buy left
  cout << add_stock(C, 1, "g", 19);
  cout << add_stock(C, 2, "g", 19);
  cout << add_stock(C, 3, "g", 19);
  cout << add_order(C, 1, "g", 1, -100);
  cout << add_order(C, 2, "g", 2, -99);
  cout << add_order(C, 3, "g", 2, -99);
  cout << add_order(C, 4, "g", 7, 101);
  // too expensive to buy
  cout << add_stock(C, 1, "h", 19);
  cout << add_order(C, 1, "h", 3, -102);
  cout << add_order(C, 3, "h", 4, 101);
  //check sell exact num
  cout << add_stock(C, 1, "i", 19);
  cout << add_order(C, 3, "i", 4, 101);
  cout << add_order(C, 1, "i", 4, -100);
  
  //check sell all, buy left
  cout << add_stock(C, 1, "j", 19);
  cout << add_order(C, 3, "j", 4, 101);
  //cout << add_order(C, 1, "j", 3, -100);
  //check buy all, sell left
  cout << add_stock(C, 4, "k", 19);
  cout << add_order(C, 2, "k", 2, 99);
  cout << add_order(C, 3, "k", 2, 99);
  cout << add_order(C, 4, "k", 7, -98);
  // too cheap to sell
  cout << add_stock(C, 1, "l", 19);
  cout << add_order(C, 3, "l", 4, 101);
  cout << add_order(C, 1, "l", 3, -102);
  //check query many exec
  cout<<query(C, 17);
  //check query error
  cout<<query(C, 24);
  //cancel refund  
  cout<<cancel(C, 8);//money
  cout<<cancel(C, 3);//stock
  //cancel twice no refund
  cout<<cancel(C, 8);//money
  //cancel error
  cout<<cancel(C, 25);

  C->disconnect();
  return 0;
}
