#include "query_funcs.h"
#include "tool.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>

void runSQL(string sql, connection *C){
  work W(*C);
  W.exec(sql);
  W.commit();
}

result selectSQL(connection *C, string sql){
  nontransaction N(*C);
  result R( N.exec( sql ));
  N.commit();
  return R;
}

string quoteStr(connection *C, string s){
  nontransaction N(*C);
  return N.quote(s);
}

///////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////

string add_account(connection *C, int account_id, float balance){
  string sql1 = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID= "+ to_string(account_id);
  result R=selectSQL(C,sql1);
  if(R.size()!=0){
    return "  <error id=\""+to_string(account_id)+"\">Account already exists</error>\n";
  }
  if(balance<=0){
    return "  <error id=\""+to_string(account_id)+"\">Balance cannot be smaller or equal to 0</error>\n";
  }

  string sql2 = "INSERT INTO ACCOUNT (ACCOUNT_ID, BALANCE) VALUES (" 
                + to_string(account_id) + "," 
                + to_string(balance) + ");";
  runSQL(sql2,C);
  return "  <created id=\""+to_string(account_id)+"\"/>\n";
}

///////////////////////////////////////////////////////////////////////////////////////

string add_stock(connection *C, int account_id, string symbol, int amount){
  if(amount<=0){
    return "    <error sym=\""+to_string(symbol)+"\" id=\""+to_string(account_id)+"\">Amount cannot be smaller or equal to 0</error>\n";
  }
  //check account exist
  string sql1 = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID= "+ quoteStr(C, to_string(account_id));
  result R=selectSQL(C, sql1 );
  if(R.size()==0){
    return "    <error sym=\""+to_string(symbol)+"\" id=\""+to_string(account_id)+"\">Account does not exist</error>\n";
  }
  
  string sql2 = "SELECT STOCK_ID FROM STOCK \
                WHERE ACCOUNT_ID= " + quoteStr(C, to_string(account_id)) + " AND SYMBOL = " + quoteStr(C, symbol);
  result R2 = selectSQL(C, sql2);
  if(R2.size()==0){//stock does not exist->insert
    string sql = "INSERT INTO STOCK (ACCOUNT_ID, SYMBOL, AMOUNT) VALUES (" 
                  + to_string(account_id) + "," 
                  + quoteStr(C,symbol) + "," 
                  + to_string(amount) + ");";
    runSQL(sql,C);
  }
  else{//stock exist->update
    string sql = "UPDATE STOCK \
                  SET AMOUNT=AMOUNT+" + to_string(amount) +
                  "WHERE STOCK_ID = " + R2[0][0].as<string>();
    runSQL(sql,C);
  }
  return "  <created sym=\""+to_string(symbol)+"\" id=\""+to_string(account_id)+"\"/>\n";
}

///////////////////////////////////////////////////////////////////////////////////////

string query_open(connection *C, int order_id){
  string ans="";
  string sql1 = "SELECT AMOUNT \
                FROM ORDERS \
                WHERE ORDER_ID = " + to_string(order_id) + " AND STATES = 'open'";
  result R=selectSQL(C, sql1);

  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    ans+="    <open shares="+c[0].as<string>()+"/>\n";
  }
  return ans;
}

string query_cancel(connection *C, int order_id){
  string ans="";
  string sql1 = "SELECT AMOUNT, TIMESEC \
                FROM ORDERS \
                WHERE ORDER_ID = " + to_string(order_id) + " AND STATES = 'cancel'";
  result R=selectSQL(C, sql1);

  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    ans+="    <canceled shares="+c[0].as<string>()+" time="+c[1].as<string>()+"/>\n";
  }
  return ans;
}

string query_execute(connection *C, int order_id){
  string ans="";
  string sql1 = "SELECT AMOUNT, TIMESEC, PRICE \
                FROM ORDERS \
                WHERE ORDER_ID = " + to_string(order_id) + " AND STATES = 'execute'";
  result R=selectSQL(C, sql1);

  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    ans+="    <executed shares="+c[0].as<string>()+" price="+c[2].as<string>()+" time="+c[1].as<string>()+"/>\n";
  }
  return ans;
}

string query_error(connection *C, int order_id){
  string sql1 = "SELECT * \
                FROM ORDERS \
                WHERE ORDER_ID = " + to_string(order_id);
  result R=selectSQL(C, sql1);

  if(R.size()==0){
    return "  <error id=\""+to_string(order_id)+"\">No such trans_id</error>\n";
  }
  return "";
}

string query_body(connection *C, int order_id){
  return query_open(C,order_id)+query_cancel(C,order_id)+query_execute(C,order_id);
}

string query(connection *C, int order_id){
  string ans=query_error(C,order_id);
  if(ans!=""){
    return ans;
  }
  return "  <status id=\""+to_string(order_id)+"\">\n" + query_body(C,order_id) + "  </status>\n";
}

///////////////////////////////////////////////////////////////////////////////////////

void refundMoney(connection *C, int account_id, float refund){
  string sql="UPDATE ACCOUNT \
              SET BALANCE = BALANCE + " + to_string(refund) +
              "WHERE ACCOUNT_ID = " + to_string(account_id);
  runSQL(sql, C);
}

void refundStock(connection *C, int account_id, string symbol, int amount){
  add_stock(C, account_id, symbol, amount);
}

string cancel(connection *C, int order_id){
  string ans=query_error(C,order_id);
  if(ans!=""){
    return ans;
  }
  //already canceled
  string sql1 = "SELECT * \
                FROM ORDERS \
                WHERE ORDER_ID = " + to_string(order_id) + " AND STATES = " + quoteStr(C, "open");
  result R=selectSQL(C, sql1);
  if(R.size()<=0){
    return "  <canceled id=\""+to_string(order_id)+"\">\n" + query_body(C,order_id) + "  </canceled>\n";
  }
  //update order to cancel
  string sql2="UPDATE ORDERS \
              SET STATES=" + quoteStr(C, "cancel") +
              "WHERE ORDER_ID = " + to_string(order_id) + " AND STATES = " + quoteStr(C, "open");
  runSQL(sql2, C);
  //get refund value
  string sql3 = "SELECT * \
                FROM ORDERS \
                WHERE ORDER_ID = " + to_string(order_id) + " AND STATES = " + quoteStr(C, "cancel");
  result R2=selectSQL(C, sql3);
  //refund
  if(R[0][5].as<string>()=="buy"){
    refundMoney(C, R2[0][1].as<int>(), R2[0][3].as<int>()*R2[0][4].as<float>());
  }
  else{
    refundStock(C, R2[0][1].as<int>(), R2[0][2].as<string>(), R2[0][3].as<int>());
  }

  return "  <canceled id=\""+to_string(order_id)+"\">\n" + query_body(C,order_id) + "  </canceled>\n";
}


///////////////////////////////////////////////////////////////////////////////////////

void insert_order(connection *C, int order_id, int account_id, string symbol, int amount, float price, string type, string states){
  if(order_id==0){
    string sql = "INSERT INTO ORDERS (ACCOUNT_ID, SYMBOL, AMOUNT, PRICE, TYPES, STATES, TIMESEC) VALUES (" 
                  + to_string(account_id) + "," 
                  + quoteStr(C, symbol) + "," 
                  + to_string(amount) + "," 
                  + to_string(price) + "," 
                  + quoteStr(C, type) + "," 
                  + quoteStr(C, states) + "," 
                  + to_string(getTime())+");";
    runSQL(sql, C);
  }
  else{
    string sql = "INSERT INTO ORDERS (ORDER_ID, ACCOUNT_ID, SYMBOL, AMOUNT, PRICE, TYPES, STATES, TIMESEC) VALUES (" 
                  + to_string(order_id) + "," 
                  + to_string(account_id) + "," 
                  + quoteStr(C, symbol) + "," 
                  + to_string(amount) + "," 
                  + to_string(price) + "," 
                  + quoteStr(C, type) + "," 
                  + quoteStr(C, states) + "," 
                  +to_string(getTime())+");";
    runSQL(sql, C);

  }
}

void buyAll(connection *C, result::const_iterator buy, result::const_iterator sell, float price){
  //sell update amount
  string sql3="UPDATE ORDERS \
              SET AMOUNT=AMOUNT-" + buy[3].as<string>() +
              "WHERE UNIQUE_ID = " + quoteStr(C,sell[8].as<string>());
  runSQL(sql3, C);
  // sell insert executed (price depends)
  insert_order(C, sell[0].as<int>(), sell[1].as<int>(), sell[2].as<string>(), buy[3].as<int>(), price, "sell", "execute");
  // buy update execute (price depends)
  string sql4="UPDATE ORDERS \
              SET TIMESEC= "+to_string(getTime())+", STATES = 'execute', PRICE = "+ to_string(price) +
              "WHERE UNIQUE_ID = " + quoteStr(C,buy[8].as<string>());
  runSQL(sql4, C);
  refundMoney(C, sell[1].as<int>(), price*buy[3].as<int>());
  refundStock(C, buy[1].as<int>(), buy[2].as<string>(), buy[3].as<int>());
}

void sellAll(connection *C, result::const_iterator buy, result::const_iterator sell, float price){
  //buy update amount
  string sql3="UPDATE ORDERS \
              SET AMOUNT=AMOUNT-" + sell[3].as<string>() +
              "WHERE UNIQUE_ID = " + quoteStr(C,buy[8].as<string>());
  runSQL(sql3, C);
  //buy insert execute (price depends)
  insert_order(C, buy[0].as<int>(), buy[1].as<int>(), buy[2].as<string>(), sell[3].as<int>(), price, "buy", "execute");
  //sell update execute (price depends)
  string sql4="UPDATE ORDERS \
              SET TIMESEC= "+to_string(getTime())+", STATES = 'execute', PRICE = "+ to_string(price) +
              "WHERE UNIQUE_ID = " + quoteStr(C,sell[8].as<string>());
  runSQL(sql4, C);
  refundMoney(C, sell[1].as<int>(), price*buy[3].as<int>());
  refundStock(C, buy[1].as<int>(), buy[2].as<string>(), buy[3].as<int>());
}

int add_buy_order(connection *C, int account_id, string symbol, int amount, float price, string states){
  insert_order(C, 0, account_id, symbol, amount, price, "buy", states);
  //get sell orders
  string sql1 = "SELECT * \
                FROM ORDERS \
                WHERE TYPES= 'sell' AND SYMBOL= " + quoteStr(C, symbol) +" AND PRICE <= " + to_string(price) + " AND STATES = 'open'\
                ORDER BY PRICE ASC, TIMESEC ASC";
  result R_sell=selectSQL(C, sql1);
  
  result num=selectSQL(C, "SELECT lastval();");
  result::const_iterator c = R_sell.begin();
  while(true){
    //get buy order
    string sql2 = "SELECT * \
                  FROM ORDERS \
                  WHERE UNIQUE_ID = " + num[0][0].as<string>();
    result R_buy=selectSQL(C, sql2);

    if(amount == 0){
      string sql3="DELETE FROM ORDERS\
                  WHERE ORDER_ID = " + to_string(R_buy.begin()[0]) + " AND AMOUNT = 0";
      runSQL(sql3, C);
      return R_buy[0][0].as<int>();
    }
    if(c == R_sell.end()){ // no more to sell
      return R_buy[0][0].as<int>();
    }
    if(amount < c[3].as<int>()){
      buyAll(C, R_buy.begin(),c, c[4].as<float>());
      refundMoney(C, R_buy[0][1].as<int>(), (R_buy[0][4].as<float>()-c[4].as<float>())*amount);
      return R_buy[0][0].as<int>();      
    }
    else{
      sellAll(C, R_buy.begin(),c, c[4].as<float>());
      amount-=c[3].as<int>();
      refundMoney(C, R_buy[0][1].as<int>(), (R_buy[0][4].as<float>()-c[4].as<float>())*c[3].as<int>());
      //refund $
    }
    ++c;
  }
}

int add_sell_order(connection *C, int account_id, string symbol, int amount, float price, string states){
  insert_order(C, 0, account_id, symbol, amount, price, "sell", states);
  //get buy orders
  string sql1 = "SELECT * \
                FROM ORDERS \
                WHERE TYPES= 'buy' AND SYMBOL= " + quoteStr(C, symbol) +" AND PRICE >= " + to_string(price) + " AND STATES = 'open'\
                ORDER BY PRICE DESC, TIMESEC ASC";
  result R_buy=selectSQL(C, sql1);
  
  result num=selectSQL(C, "SELECT lastval();");
  result::const_iterator c = R_buy.begin();
  while(true){
    //get sell order
    string sql2 = "SELECT * \
                  FROM ORDERS \
                  WHERE UNIQUE_ID = " + num[0][0].as<string>();
    result R_sell=selectSQL(C, sql2);

    if(amount == 0){ //done
      string sql3="DELETE FROM ORDERS\
                  WHERE ORDER_ID = " + to_string(R_sell.begin()[0]) + " AND AMOUNT = 0";
      runSQL(sql3, C);
      return R_sell[0][0].as<int>();
    }
    if(c == R_buy.end()){// no more to buy
      return R_sell[0][0].as<int>();
    }
    if(amount < c[3].as<int>()){ // sell all, buy left
      sellAll(C, c, R_sell.begin(), c[4].as<float>());
      return R_sell[0][0].as<int>();
    }
    else{ // buy all
      buyAll(C, c, R_sell.begin(), c[4].as<float>());
      amount-=c[3].as<int>();
    }
    ++c;
  }
  
}

string add_order(connection *C, int account_id, string symbol, int amount, float price){
  string sql1 = "SELECT ACCOUNT_ID, BALANCE FROM ACCOUNT WHERE ACCOUNT_ID= "+ to_string(account_id);
  result R=selectSQL(C, sql1);
  if(R.size()==0){
    return "  <error sym=\""+to_string(symbol)+"\" amount=\""+to_string(amount)+"\" limit=\""+to_string(static_cast<int>(price))+"\">Account does not exist</error>\n";
  }
  if(amount<=0){
    return "  <error sym=\""+to_string(symbol)+"\" amount=\""+to_string(amount)+"\" limit=\""+to_string(static_cast<int>(price))+"\">Amount cannot be smaller or equal to 0</error>\n";
  }
  string type= "buy";
  if(price<0){
    type="sell";
    price=-price;
  }
  if(type=="buy"){
    if(price*amount>=R.begin()[1].as<int>()){
      return "  <error sym=\""+to_string(symbol)+"\" amount=\""+to_string(amount)+"\" limit=\""+to_string(static_cast<int>(price))+"\">Not enough money</error>\n";
    }
    else{
      string sql2="UPDATE ACCOUNT \
                    SET BALANCE = BALANCE-"+to_string(price*amount)+" \
                    WHERE ACCOUNT_ID= "+ to_string(account_id);
      runSQL(sql2,C);
      int n = add_buy_order(C, account_id, symbol, amount, price, "open");
      return "  <opened sym=\""+to_string(symbol)+"\" amount=\""+to_string(amount)+"\" limit=\""+to_string(static_cast<int>(price))+"\" id=\""+to_string(n)+"\"/>\n";  
    }
  }
  else{//sell
    string sql3 = "SELECT STOCK_ID, AMOUNT \
                    FROM STOCK \
                    WHERE ACCOUNT_ID= "+ to_string(account_id) + " AND SYMBOL = " + quoteStr(C,symbol);
    result R2=selectSQL(C,sql3);
    if(R2.size()<1 || amount > R2.begin()[1].as<int>()){
      return "  <error sym=\""+to_string(symbol)+"\" amount=\""+to_string(amount)+"\" limit=\""+to_string(static_cast<int>(price))+"\">Not enough stocks</error>\n";
    }
    else{
      string sql4="UPDATE STOCK \
                  SET AMOUNT = AMOUNT-"+ to_string(amount)+" \
                  WHERE ACCOUNT_ID= "+ to_string(account_id) + " AND SYMBOL = " + quoteStr(C,symbol);
      runSQL(sql4,C);            
      int n = add_sell_order(C, account_id, symbol, amount, price, "open");
      return "  <opened sym=\""+to_string(symbol)+"\" amount=\""+to_string(amount)+"\" limit=\""+to_string(static_cast<int>(price))+"\" id=\""+to_string(n)+"\"/>\n";
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