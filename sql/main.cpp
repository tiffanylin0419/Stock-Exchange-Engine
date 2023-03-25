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
/*

void insertState(string fileName, connection *C){
  string line, name;
  int state_id;
  ifstream f (fileName.c_str());
  if (f.is_open()){
      while(getline(f,line)){
        stringstream ss;
        ss << line;
        ss >> state_id >> name;
        add_state(C, name);
      }
      f.close();
  }
  else{
    cout << "Cannot open " + fileName <<endl;
    exit(1);
  }
  //cout << "insert " <<fileName<< endl;
}

void insertTeam(string fileName, connection *C){
  //string name, int state_id, int color_id, int wins, int losses
  string line, name;
  int team_id, state_id, color_id, wins, losses;
  ifstream f (fileName.c_str());
  if (f.is_open()){
      while(getline(f,line)){
        stringstream ss;
        ss << line;
        ss >> team_id >> name >> state_id >> color_id >> wins >> losses;
        add_team(C, name, state_id, color_id, wins, losses);
      }
      f.close();
  }
  else{
    cout << "Cannot open " + fileName <<endl;
    exit(1);
  }
  //cout << "insert " <<fileName<< endl;
}


void insertPlayer(string fileName, connection *C){
  //connection *C, int team_id, int jersey_num, string first_name, string last_name,
  //int mpg, int ppg, int rpg, int apg, double spg, double bpg)
  string line, first_name, last_name;
  int player_id, team_id, jersey_num, mpg, ppg, rpg, apg;
  double spg, bpg;
  ifstream f (fileName.c_str());
  if (f.is_open()){
      while(getline(f,line)){
        stringstream ss;
        ss << line;
        ss >> player_id >> team_id >> jersey_num >> first_name >> last_name >> mpg >> ppg >> rpg >> apg >> spg >> bpg;
        add_player(C, team_id, jersey_num, first_name, last_name, mpg, ppg, rpg, apg, spg, bpg);
      }
      f.close();
  }
  else{
    cout << "Cannot open " + fileName <<endl;
    exit(1);
  }
  //cout << "insert " <<fileName<< endl;
}
*/
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
  createTable("account.sql", C);
  insertAccount("account.txt", C);
  //Close database connection
  C->disconnect();
  return 0;
}

