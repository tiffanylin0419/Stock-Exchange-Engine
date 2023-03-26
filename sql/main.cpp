#include <iostream>
#include <pqxx/pqxx>
#include <fstream>

#include "query_funcs.h"

using namespace std;
using namespace pqxx;

void deleteTable(connection *C, string tableName){
  string sql = "DROP TABLE IF EXISTS " + tableName + " CASCADE;";
  runSQL(sql,C);
}

void createTable(string fileName, connection *C){
  string sql="";
  string line;
  ifstream f (fileName.c_str());
  if (f.is_open()){
      while(getline(f,line)){
      	sql.append(line);
      }
      f.close();
  }
  else{
    cout << "Cannot open " + fileName <<endl;
    exit(1);
  }
  runSQL(sql,C);
}

void insertAccount(string fileName, connection *C){
  string line;
  int account_id;
  float balance;
  ifstream f (fileName.c_str());
  if (f.is_open()){
      while(getline(f,line)){
        stringstream ss;
        ss << line;
        ss >> account_id >> balance;
        add_account(C, account_id , balance);
      }
      f.close();
  }
  else{
    cout << "Cannot open " + fileName <<endl;
    exit(1);
  }
}

void insertStock(string fileName, connection *C){
  string line, symbol;
  int account_id, amount;
  ifstream f (fileName.c_str());
  if (f.is_open()){
      while(getline(f,line)){
        stringstream ss;
        ss << line;
        ss >> account_id >> symbol >> amount;
        add_stock(C, account_id, symbol, amount);
      }
      f.close();
  }
  else{
    cout << "Cannot open " + fileName <<endl;
    exit(1);
  }
}

void insertOrder(string fileName, connection *C){
  string line, symbol, states;
  int account_id, amount;
  float price;
  ifstream f (fileName.c_str());
  if (f.is_open()){
      while(getline(f,line)){
        stringstream ss;
        ss << line;
        ss >> account_id >> symbol >> amount >> price >> states;
        add_order(C, account_id, symbol, amount, price, states);
      }
      f.close();
  }
  else{
    cout << "Cannot open " + fileName <<endl;
    exit(1);
  }
}

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
  insertAccount("account.txt", C);
  insertStock("stock.txt", C);
  insertOrder("order.txt", C);
  C->disconnect();
  return 0;
}

