#include "query_funcs.h"

void runSQL(string sql, connection *C){
  work W(*C);
  W.exec(sql);
  W.commit();
}

result selectSQL(connection *C, string sql){
  nontransaction N(*C);
  result R( N.exec( sql ));
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
  if(balance<=0){
    return "  <error id=\""+to_string(account_id)+"\">Balance cannot be smaller or equal to 0</error>\n";
  }
  string sql1 = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID= "+ to_string(account_id);
  result R=selectSQL(C, sql1);
  if(R.size()!=0){
    return "  <error id=\""+to_string(account_id)+"\">Account already exists</error>\n";
  }
  //insert if does not exist
  string sql2 = "INSERT INTO ACCOUNT (ACCOUNT_ID, BALANCE) SELECT " 
                + to_string(account_id) + "," 
                + to_string(balance) + ""
                +"WHERE NOT EXISTS (SELECT 1 FROM ACCOUNT WHERE ACCOUNT_ID=" + to_string(account_id) + ");";
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
  // if exist, update, if not, insert
  string sql = "INSERT INTO STOCK (ACCOUNT_ID, SYMBOL, AMOUNT) VALUES (" 
                  + to_string(account_id) + "," 
                  + quoteStr(C,symbol) + "," 
                  + to_string(amount) + ") "
                  + "ON CONFLICT (ACCOUNT_ID, SYMBOL) DO UPDATE SET AMOUNT = STOCK.AMOUNT + " + to_string(amount)+";";
  runSQL(sql,C);
  return "  <created sym=\""+to_string(symbol)+"\" id=\""+to_string(account_id)+"\"/>\n";
}

///////////////////////////////////////////////////////////////////////////////////////

string query_open(work& W, connection *C, int order_id){
  string ans="";
  string sql1 = "SELECT AMOUNT \
                FROM ORDERS \
                WHERE ORDER_ID = " + to_string(order_id) + " AND STATES = 'open'";
  result R(W.exec(sql1));

  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    ans+="    <open shares="+c[0].as<string>()+"/>\n";
  }
  return ans;
}

string query_cancel(work& W, connection *C, int order_id){
  string ans="";
  string sql1 = "SELECT AMOUNT, TIMESEC \
                FROM ORDERS \
                WHERE ORDER_ID = " + to_string(order_id) + " AND STATES = 'cancel'";
  result R(W.exec(sql1));

  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    ans+="    <canceled shares="+c[0].as<string>()+" time="+c[1].as<string>()+"/>\n";
  }
  return ans;
}

string query_execute(work& W, connection *C, int order_id){
  string ans="";
  string sql1 = "SELECT AMOUNT, TIMESEC, PRICE \
                FROM ORDERS \
                WHERE ORDER_ID = " + to_string(order_id) + " AND STATES = 'execute'";
  result R(W.exec(sql1));

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
  work W(*C);
  return query_open(W, C,order_id)+query_cancel(W, C,order_id)+query_execute(W, C,order_id);
  W.commit();
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

string cancel(connection *C, int account_id, int order_id){
  string ans=query_error(C,order_id);
  if(ans!=""){
    return ans;
  }
  //already canceled
  string sql1 = "SELECT * \
                FROM ORDERS \
                WHERE ORDER_ID = " + to_string(order_id) + " AND STATES = " + quoteStr(C, "open") 
                + " FOR UPDATE";
  result R=selectSQL(C, sql1);
  if(R.size()<=0){
    return "  <canceled id=\""+to_string(order_id)+"\">\n" + query_body(C,order_id) + "  </canceled>\n";
  }
  if(R[0][1].as<int>()!=account_id){
    return "  <error id=\""+to_string(order_id)+"\">Account does not own this transaction</error>\n";
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
                ORDER BY PRICE ASC, TIMESEC ASC"
                + " FOR UPDATE";
  result R_sell=selectSQL(C, sql1);
  
  result num=selectSQL(C, "SELECT lastval() FOR UPDATE;");
  result::const_iterator c = R_sell.begin();
  while(true){
    //get buy order
    string sql2 = "SELECT * \
                  FROM ORDERS \
                  WHERE UNIQUE_ID = " + num[0][0].as<string>()
                  + " FOR UPDATE";
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
                ORDER BY PRICE DESC, TIMESEC ASC"
                + " FOR UPDATE";
  result R_buy=selectSQL(C, sql1);
  
  result num=selectSQL(C, "SELECT lastval() FOR UPDATE;");
  result::const_iterator c = R_buy.begin();
  while(true){
    //get sell order
    string sql2 = "SELECT * \
                  FROM ORDERS \
                  WHERE UNIQUE_ID = " + num[0][0].as<string>()
                  + " FOR UPDATE";
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
  if(amount<=0){
    return "  <error sym=\""+to_string(symbol)+"\" amount=\""+to_string(amount)+"\" limit=\""+to_string(static_cast<int>(price))+"\">Amount cannot be smaller or equal to 0</error>\n";
  }

  string sql1 = "SELECT ACCOUNT_ID, BALANCE FROM ACCOUNT WHERE ACCOUNT_ID= "+ to_string(account_id) + " FOR UPDATE";
  result R=selectSQL(C, sql1);
  if(R.size()==0){
    return "  <error sym=\""+to_string(symbol)+"\" amount=\""+to_string(amount)+"\" limit=\""+to_string(static_cast<int>(price))+"\">Account does not exist</error>\n";
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
                    WHERE ACCOUNT_ID= "+ to_string(account_id) + " AND SYMBOL = " + quoteStr(C,symbol)
                    + " FOR UPDATE";
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

