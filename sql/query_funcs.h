#include <iostream>
#include <pqxx/pqxx>
#include <string>

using namespace std;
using namespace pqxx;

#ifndef _QUERY_FUNCS_
#define _QUERY_FUNCS_


void deleteTable(connection *C, string tableName);
void createTable(string fileName, connection *C);

string add_account(connection *C, int account_id, float balance);
string add_stock(connection *C, int account_id, string symbol, int amount);
string add_order(connection *C, int account_id, string symbol, int amount, float price, string states);

/*
void insertAccount(string fileName, connection *C);
void insertStock(string fileName, connection *C);
void insertOrder(string fileName, connection *C);
*/




#endif //_QUERY_FUNCS_
