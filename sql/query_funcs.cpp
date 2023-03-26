#include "query_funcs.h"
#include "tool.cpp"
#include <iostream>
#include <iomanip>
#include <fstream>


void runSQL(string sql, connection *C){
  work W(*C);
  W.exec(sql);
  W.commit();
}

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

string add_account(connection *C, int account_id, float balance){
  nontransaction N(*C);
  string sql1 = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID= "+ N.quote(account_id);
  result R( N.exec( sql1 ));
  if(R.size()!=0){
    return "<error id=\""+to_string(account_id)+"\">Account already exists</error>\n";
  }
  N.commit();
  string sql2 = "INSERT INTO ACCOUNT (ACCOUNT_ID, BALANCE) VALUES (" 
                + to_string(account_id) + "," 
                + to_string(balance) + ");";
  runSQL(sql2,C);
  return "<created id=\""+to_string(account_id)+"\"/>\n";
}

string quoteStr(connection *C, string s){
  nontransaction N(*C);
  return N.quote(s);
}

string add_stock(connection *C, int account_id, string symbol, int amount){
  nontransaction N(*C);
  string sql1 = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID= "+ N.quote(account_id);
  result R( N.exec( sql1 ));
  if(R.size()==0){
    return "<error sym=\""+to_string(symbol)+"\" id=\""+to_string(account_id)+"\">Account does not exist</error>\n";
  }
  N.exec(sql1);
  N.commit();

  string sql = "INSERT INTO STOCK (ACCOUNT_ID, SYMBOL, AMOUNT) VALUES (" 
                + to_string(account_id) + "," 
                + quoteStr(C,symbol) + "," 
                + to_string(amount) + ");";
  runSQL(sql,C);
  return "<created sym=\""+to_string(symbol)+"\" id=\""+to_string(account_id)+"\"/>\n";
}

string add_buy_order(connection *C, int account_id, string symbol, int amount, float price, string states){
  //todo
  /*string sql = "INSERT INTO ORDERS (ACCOUNT_ID, SYMBOL, AMOUNT, PRICE, TYPES, STATES, TIME) VALUES (" 
                + to_string(account_id) + "," 
                + W.quote(symbol) + "," 
                + to_string(amount) + "," 
                + to_string(price) + "," 
                + W.quote(type) + "," 
                + W.quote(states) + "," 
                + "NOW());";
  W.exec(sql);*/
  return "<opened sym=\""+to_string(symbol)+"\" amount=\""+to_string(amount)+"\" limit=\""+float_to_string(price)+"\" id=\""+to_string(account_id)+"\"/>\n";
}

string add_sell_order(connection *C, int account_id, string symbol, int amount, float price, string states){
  //todo
  return "<opened sym=\""+to_string(symbol)+"\" amount=\""+to_string(amount)+"\" limit=\""+float_to_string(-price)+"\" id=\""+to_string(account_id)+"\"/>\n";
}

string add_order(connection *C, int account_id, string symbol, int amount, float price, string states){
  work W(*C);
  string sql1 = "SELECT ACCOUNT_ID, BALANCE FROM ACCOUNT WHERE ACCOUNT_ID= "+ W.quote(account_id);
  result R( W.exec( sql1 ));
  if(R.size()==0){
    return "<error sym=\""+to_string(symbol)+"\" amount=\""+to_string(amount)+"\" limit=\""+float_to_string(price)+"\">Account does not exist</error>\n";
  }
  string type= "buy";
  if(price<0){
    type="sell";
    price=-price;
  }
  if(type=="buy"){
    if(price*amount>=R.begin()[1].as<int>()){
      return "<error sym=\""+to_string(symbol)+"\" amount=\""+to_string(amount)+"\" limit=\""+float_to_string(price)+"\">Not enough money</error>\n";
    }
    else{
      string sql2="UPDATE ACCOUNT \
                    SET BALANCE = BALANCE-"+to_string(price*amount)+" \
                    WHERE ACCOUNT_ID= "+ W.quote(account_id);
      W.exec(sql2);
      W.commit();
      return add_buy_order(C, account_id, symbol, amount, price, states);
    }
  }
  else{//sell
    string sql3 = "SELECT STOCK_ID, AMOUNT \
                    FROM STOCK \
                    WHERE ACCOUNT_ID= "+ W.quote(account_id) + " AND SYMBOL = " + W.quote(symbol);
    result R2( W.exec( sql3 ));
    if(R2.size()<1 || amount > R2.begin()[1].as<int>()){
      return "<error sym=\""+to_string(symbol)+"\" amount=\""+to_string(amount)+"\" limit=\""+float_to_string(-price)+"\">Not enough stocks</error>\n";
    }
    else{
      string sql4="UPDATE STOCK \
                  SET AMOUNT = AMOUNT-"+to_string(amount)+" \
                  WHERE ACCOUNT_ID= "+ W.quote(account_id);
      W.exec(sql4);
      W.commit();
      return add_sell_order(C, account_id, symbol, amount, price, states);
    }
  }  
}

/*
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
*/

/*
void addSizeCondition(string * sql, int use, bool* notFiltered, string name, int min, int max){
  if(use){
    if(*notFiltered){
      sql->append("WHERE ");
      *notFiltered=false;
    }
    else{
      sql->append("AND ");
    }
    sql->append(name+" >= "+to_string(min)+" AND "+name+" <= "+to_string(max)+" ");
  }
}
void addSizeCondition(string * sql, int use, bool* notFiltered, string name, double min, double max){
  if(use){
    if(*notFiltered){
      sql->append("WHERE ");
      *notFiltered=false;
    }
    else{
      sql->append("AND ");
    }
    sql->append(name+" >= "+to_string(min)+" AND "+name+" <= "+to_string(max)+" ");
  }
}

void query1(connection *C,
  int use_mpg, int min_mpg, int max_mpg,
  int use_ppg, int min_ppg, int max_ppg,
  int use_rpg, int min_rpg, int max_rpg,
  int use_apg, int min_apg, int max_apg,
  int use_spg, double min_spg, double max_spg,
  int use_bpg, double min_bpg, double max_bpg
  )
{
  string sql = "SELECT * FROM PLAYER ";  
  bool notFiltered=true;
  addSizeCondition(&sql, use_mpg, &notFiltered, "MPG", min_mpg, max_mpg);
  addSizeCondition(&sql, use_ppg, &notFiltered, "PPG", min_ppg, max_ppg);
  addSizeCondition(&sql, use_rpg, &notFiltered, "RPG", min_rpg, max_rpg);
  addSizeCondition(&sql, use_apg, &notFiltered, "APG", min_apg, max_apg);
  addSizeCondition(&sql, use_spg, &notFiltered, "SPG", min_spg, max_spg);
  addSizeCondition(&sql, use_bpg, &notFiltered, "BPG", min_bpg, max_bpg);

  nontransaction N(*C);
  result R( N.exec( sql ));
  cout <<"PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG" << endl;
  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    cout << c[0].as<int>() << " ";
    cout << c[1].as<int>() << " ";
    cout << c[2].as<int>() << " ";
    cout << c[3].as<string>() << " ";
    cout << c[4].as<string>() << " ";
    cout << c[5].as<int>() << " ";
    cout << c[6].as<int>() << " ";
    cout << c[7].as<int>() << " ";
    cout << c[8].as<int>() << " ";
    cout << std::fixed << std::setprecision(1) << c[9].as<double>() << " ";
    cout << std::fixed << std::setprecision(1) << c[10].as<double>() << endl;
  }
}


void query2(connection *C, string team_color)
{
  work W(*C);
  string sql = "SELECT TEAM.NAME FROM TEAM, COLOR WHERE TEAM.COLOR_ID=COLOR.COLOR_ID AND COLOR.NAME = "+ W.quote(team_color);
  W.commit();

  nontransaction N(*C);
  result R( N.exec( sql ));
  cout <<"NAME" << endl;
  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    cout << c[0].as<string>() << endl;
  }
}


void query3(connection *C, string team_name)
{
  work W(*C);
  string sql = "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME FROM PLAYER, TEAM WHERE TEAM.TEAM_ID=PLAYER.TEAM_ID AND TEAM.NAME = "+ W.quote(team_name)+" ORDER BY PLAYER.PPG DESC";
  W.commit();

  nontransaction N(*C);
  result R( N.exec( sql ));
  cout <<"FIRST_NAME LAST_NAME" << endl;
  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    cout << c[0].as<string>() << ' ';
    cout << c[1].as<string>() << endl;
  }
}


void query4(connection *C, string team_state, string team_color)
{
  work W(*C);
  string sql = "SELECT PLAYER.UNIFORM_NUM, PLAYER.FIRST_NAME, PLAYER.LAST_NAME FROM PLAYER, TEAM, STATE, COLOR WHERE TEAM.TEAM_ID=PLAYER.TEAM_ID AND TEAM.STATE_ID=STATE.STATE_ID AND TEAM.COLOR_ID=COLOR.COLOR_ID AND STATE.NAME = "+ W.quote(team_state)+" AND COLOR.NAME = "+ W.quote(team_color);
  W.commit();

  nontransaction N(*C);
  result R( N.exec( sql ));
  cout <<"UNIFORM_NUM FIRST_NAME LAST_NAME" << endl;
  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    cout << c[0].as<int>() << ' ';
    cout << c[1].as<string>() << ' ';
    cout << c[2].as<string>() << endl;
  }
}


void query5(connection *C, int num_wins)
{
  work W(*C);
  string sql = "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME, TEAM.NAME, TEAM.WINS FROM PLAYER, TEAM WHERE TEAM.TEAM_ID=PLAYER.TEAM_ID AND TEAM.WINS > "+ to_string(num_wins);
  W.commit();

  nontransaction N(*C);
  result R( N.exec( sql ));
  cout <<"FIRST_NAME LAST_NAME NAME WINS" << endl;
  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    cout << c[0].as<string>() << ' ';
    cout << c[1].as<string>() << ' ';
    cout << c[2].as<string>() << ' ';
    cout << c[3].as<int>() << endl;
  }
}
*/