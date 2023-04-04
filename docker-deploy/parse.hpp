#ifndef PARSER_HPP
#define PARSER_HPP

#include "pugixml/pugixml.hpp"
#include "query_funcs.h"
//#include "../sql/query_funcs.cpp"
//#include "../sql/tool.h"
//#include "../sql/tool.cpp"
//#include "../sql/head.h"
//#include <query_funcs.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <string>
#include <algorithm>
//#include <climits>
//#include <vector>
//#include <cstring>
//#include <ctime>
//#include <error.h>
//#include <fstream>
//#include <mutex>
//#include <sstream>
using namespace std;

//////main function//////
string receive(int &client_fd, int &l);
void send_response(int &client_fd, string &response_message);
int process_create(pugi::xml_document &request_doc, string &response,connection* C);
int process_transaction(pugi::xml_document &request_doc, string &response,connection *C);

//////small function/////
int getAccount_ID(pugi::xml_node &child);
float getBalance(pugi::xml_node &child);
std :: string getSym(pugi::xml_node &child);
int getNum(pugi::xml_node &acc);
int getAmount(pugi::xml_node &child);
float getLimit(pugi::xml_node &child);
int getTrans_ID(pugi::xml_node &child);

string requestToResponse(connection *C, string request);
#endif
