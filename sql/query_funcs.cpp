#include "query_funcs.h"

#include <iostream>
#include <iomanip>


void runSQL(string sql, connection *C){
  work W(*C);
  W.exec(sql);
  W.commit();
}

/*
void add_player(connection *C, int team_id, int jersey_num, string first_name, string last_name,
                int mpg, int ppg, int rpg, int apg, double spg, double bpg)
{
  work W(*C);
  string sql = "INSERT INTO PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG) VALUES (" 
              + std::to_string(team_id) + ","
              + std::to_string(jersey_num) + ","
              + W.quote(first_name) + ","
              + W.quote(last_name) + ","
              + std::to_string(mpg) + ","
              + std::to_string(ppg) + ","
              + std::to_string(rpg) + ","
              + std::to_string(apg) + ","
              + std::to_string(spg) + ","
              + std::to_string(bpg) +");";
  W.exec(sql);
  W.commit();
}


void add_team(connection *C, string name, int state_id, int color_id, int wins, int losses)
{
  work W(*C);
  string sql = "INSERT INTO TEAM (NAME, STATE_ID, COLOR_ID, WINS, LOSSES) VALUES (" 
              + W.quote(name) + ","
              + std::to_string(state_id) + ","
              + std::to_string(color_id) + ","
              + std::to_string(wins) + ","
              + std::to_string(losses) +");";
  W.exec(sql);
  W.commit();
}


void add_state(connection *C, string name)
{
  work W(*C);
  string sql = "INSERT INTO STATE (NAME) VALUES (" + W.quote(name) + ");";
  W.exec(sql);
  W.commit();
}

*/
void add_account(connection *C, int account_id, float balance)
{
  work W(*C);
  string sql = "INSERT INTO ACCOUNT (ACCOUNT_ID, BALANCE) VALUES (" + std::to_string(account_id) + "," + std::to_string(balance) + ");";
  W.exec(sql);
  W.commit();
}
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