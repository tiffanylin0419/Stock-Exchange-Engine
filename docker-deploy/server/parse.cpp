#include "parse.hpp"
#define BUFF_SIZE 6500
//std::mutex mtx;


string receive(int &client_fd, int &l) {
  char first_buffer[BUFF_SIZE];
  memset(first_buffer, 0, sizeof(first_buffer));
  std :: string request;

  //start receiving!!!
  //cout << "start receiving" << endl;
  
  //recv(client_fd, &l, sizeof(l), 0);
  int data_len = recv(client_fd, &first_buffer, sizeof(first_buffer), 0);
  if(data_len < 0)
  {
    cerr << "Fail to receive the request." << endl;
    close(client_fd);
    return "";
  }
  std :: string first_message(first_buffer);
  request = first_message;
  //get the first line of integer message size
  int xml_bytes = 0;
  try{
    /*xml_bytes = stoi(first_message.substr(0,first_message.find('\n') + 1));
    if(xml_bytes <= 0)
    {
      cerr << "xml Bytes should be larger than 0." << endl;
      return "";
    }*/
    //cout << "xml_bytes: " << xml_bytes << endl;
    bool receive_complete = false;
    int remain_len = l - BUFF_SIZE;
    // Receive whole message in the first recv
    if(data_len < BUFF_SIZE)
    {
      receive_complete = true;
      remain_len = 0;
    }
    char buffer[BUFF_SIZE];
    while(!receive_complete)
    {
      if(remain_len <= 0)
      {
        receive_complete = true;
        break;
      }
      int data_len = recv(client_fd, &buffer, sizeof(buffer), 0);
      if(data_len < 0)
      {
        cerr << "Fail to receive whole request." << endl;
        return "";
      }
      else if(data_len == 0)
      {
        receive_complete = true;
        break;
      }
      else
      {
        remain_len -= data_len;
        request.append(buffer, data_len);
      }
    }
    //get rid of first and second line of request xml
    size_t index = request.find("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    if(index == string :: npos){
      cerr << "The version and encoding should be contained" << endl;
      return "";
    }
    request = request.substr(request.find('\n') + 1);
  }
  catch (const std::exception& e){
    cerr << "The first line of xml should contain the number of bytes." << endl;
    return "";
  }
  return request;
}

////////////////////////
/*
// send back response
void send_response(int &client_fd, string &response_message) {
  cout << "start sending response message" << endl;
  size_t total_send = 0;
  bool complete = false;
  //vector<char> res(response.begin(), response.end());
  while (!complete) {
    int sent = send(client_fd, &response_message[total_send], 1024, 0);
    if(sent == -1)
    {
      cerr << "Fail to send the response." << endl;
      return;
    }
    else if(sent == 0)
    {
      complete = true;
      break;
    }
    else if(sent < 1024)
    {
      total_send += send(client_fd, &response_message[total_send], sent, 0);
      break;
    }
    else
    {
      total_send += send(client_fd, &response_message[total_send], 1024, 0);
    }
    
    //if (sent + 1024 < res.size()) {
      //sent += send(client_fd, &(res.data()[sent]), 1024, 0);
    //} else {
      //sent += send(client_fd, &(res.data()[sent]), res.size() - sent, 0);
      //break;
    //}
    
  }
  cout << "Response done!" << endl;
  return;
}
*/
// Request xml is create() request
int getAccount_ID(pugi::xml_node &child, int& flag)
{
  int account_id = 0;
  try {
    account_id = std::stoi(child.first_attribute().value());
  }
  catch (const std::exception& e){
    std::cerr << "Error: " << "The account_id should be number." << std::endl;
    flag = -1;
    return account_id;
  }
  return account_id;
}

float getBalance(pugi::xml_node &child, int &flag)
{
    float balance = 0.0;
    try {
        pugi::xml_attribute attr = child.first_attribute();
        attr = attr.next_attribute();
        balance = std::stof(attr.value());
    } catch (const std::exception& e) {
      flag = -1;
        std::cerr << "Error: " << "The balance should be number." << std::endl;
        return -1;
    }
    return balance;
}

std :: string getSym(pugi::xml_node &child, int& flag)
{
    string sym = child.first_attribute().value();
    for (char c : sym) {
        if (!isalnum(c)) {
            std::cerr << "Error: " << "The symbol should be character or number." << std::endl;
            flag = -1;
	    return "";
        }
    }
    return sym;
}

int getNum(pugi::xml_node &acc, int& flag)
{
  int num = 0;
  try 
  {
    num = stoi(acc.child_value());
  }catch (const std::exception& e){
    std::cerr << "Error: " << "The number of stocks should be number." << std::endl;
    flag = -1;
    return 0;
  }
  return num;
}

int getAmount(pugi::xml_node &child, int& flag)
{
  int amount = 0;
  try {
    pugi::xml_attribute attr = child.first_attribute();
    attr = attr.next_attribute();
    amount = stoi(attr.value());
  }catch (const std::exception& e) {
        std::cerr << "Error: " << "The amount of stocks should be number." << std::endl;
	flag = -1;
	return 0;
  }
  return amount;
}

float getLimit(pugi::xml_node &child, int& flag)
{
  float limit = 0.0;
  try { 
        pugi::xml_attribute attr = child.first_attribute();
        attr = attr.next_attribute();
        attr = attr.next_attribute();
        //string limit_str = attr.value();
        limit = stof(attr.value());
    } catch (const std::exception& e) {
        std::cerr << "Error: " << "The price of stock should be number." << std::endl;
        flag = -1;
	return 0;
    }
    return limit;
}

int getTrans_ID(pugi::xml_node &child, int& flag)
{
  int trans_id = 0;
  try { 
        trans_id = stoi(child.first_attribute().value());
    } catch (const std::exception& e) {
        std::cerr << "Error: " << "The trans_id should be number." << std::endl;
        flag = -1;
	return 0;
    }
  return trans_id;
}

// traverse xml file, create account and symbol for each account
int process_create(pugi::xml_document &request_doc, string &response,connection* C) {
  //std::lock_guard<std::mutex> lck (mtx);

  //pugi::xml_document response_doc;
  //std :: string response;
  response += "<results>\n";
  //pugi::xml_node response_head = response_doc.append_child("results");
  pugi::xml_node request_head = request_doc.child("create");
  // process child node
  for (pugi::xml_node child : request_head.children()) {
    int account_id;
    string sym;
    float balance = 0.0;
    if(to_string(child.name()) == "account")
    {
      int flag = 0;
      account_id = getAccount_ID(child, flag);
      if(flag == -1)
      {
        response += "  <error id=\"" + to_string(account_id) + "\">Account must be number.</error>\n";
        continue;
      }
      balance = getBalance(child, flag);
      if(flag == -1)
      {
        response += "  <error id=\"" + to_string(account_id) + "\">Balance should be number.</error>\n";
        continue;
      }
	  
      ///////Add new account to database////////
      std :: string account = add_account(C,account_id, balance);
      response += account;
      //pugi::xml_parse_result account_node = response_head.append_buffer(account.c_str(), account.length());
    }
    else if(to_string(child.name()) == "symbol")
    {
      int flag = 0;
      sym = getSym(child, flag);
      for(pugi::xml_node acc: child.children())
      {
	      int child_flag = 0;
        int acc_id = getAccount_ID(acc, child_flag);
        if(child_flag == -1)
	      {
	        response += "  <error id=\"" + to_string(acc_id) + "\">Account must be number.</error>\n";
          continue;
	      }
        int amount = getNum(acc, child_flag);
        if(child_flag == -1)
	      {
	        response += "  <error id=\"" + to_string(acc_id) + "\">Num must be number.</error>\n";
          continue;
	      }
        //////Add stock to database//////
        std :: string stock = add_stock(C, acc_id, sym, amount);
	      response += stock;
        //pugi::xml_parse_result stock_node = response_head.append_buffer(stock.c_str(), stock.length());
      }
    }
    else
    {
      response += "  <error id=\"" + to_string(account_id) + "\">Wrong format! Tag should be create or symbol.</error>\n";
      cout << "Create tag is illegal." << endl;
      continue;
    }
  }
  response += "</results>";
  return 0;
}


// traverse xml file, order/query/cancel any order specified
int process_transaction(pugi::xml_document &request_doc, string &response,connection *C) {
  //std::lock_guard<std::mutex> lck (mtx);

  //pugi::xml_document response_doc;
  //std :: string response;
  response += "<results>\n";
  //pugi::xml_node response_head = response_doc.append_child("results");
  pugi::xml_node request_head = request_doc.child("transactions");
  int flag = 0;
  int account_id = getAccount_ID(request_head, flag);
  if(flag == -1)
  {
    response += "  <error id=\"" + to_string(account_id) + "\">Account must be number.</error>\n";
    return -1;
  }
  /////////check whether account_id exist or not //////////
  for (pugi::xml_node child : request_doc.child("transactions")) {
    int child_flag = 0;
    if(to_string(child.name()) == "order")
    {
      std :: string sym = getSym(child, child_flag);
      int amount = getAmount(child, child_flag);
      if(child_flag == -1)
      {
        response += "  <error id=\"" + to_string(account_id) + "\">Amount must be number.</error>\n";
        continue;
      }
      float limit = getLimit(child, child_flag);
      if(child_flag == -1)
      {
        response += "  <error id=\"" + to_string(account_id) + "\">Limit must be number.</error>\n";
        continue;
      }
      std :: string order = add_order(C, account_id, sym, amount, limit);
      response += order;
      //pugi::xml_parse_result order_node = response_head.append_buffer(order.c_str(), order.length());
    }
    else if(to_string(child.name()) == "query")
    {
      int trans_id = getTrans_ID(child, child_flag);
      if(child_flag == -1)
      {
        response += "  <error id=\"" + to_string(account_id) + "\">Trans_id must be number.</error>\n";
        continue;
      }
      string quer = query(C, trans_id);
      response += quer;
      //pugi::xml_parse_result query_node = response_head.append_buffer(quer.c_str(), quer.length());
    }
    else if(to_string(child.name()) == "cancel")
    {
      int trans_id = getTrans_ID(child, child_flag);
      if(child_flag == -1)
      {
        response += "  <error id=\"" + to_string(account_id) + "\">Trans_id must be number.</error>\n";
        continue;
      }
      string canceled = cancel(C, account_id, trans_id);
      response += canceled;
      //pugi::xml_parse_result canceled_node = response_head.append_buffer(canceled.c_str(), canceled.length());
    }
    else
    {
      response += "  <error id=\"" + to_string(account_id) + "\">Wrong format! Tag should be order, query, or cancel.</error>\n";
      cout << "Illegal Tag in Transaction" << endl;
      continue;
    }
  }
  response += "</results>";
  return 0;
}



string requestToResponse(connection *C, string request){
  pugi::xml_document request_doc;
  // load xml parser
  pugi::xml_parse_result result = request_doc.load_string(request.c_str());
  std :: string response = "";
  if (!result || request == "") {
    // error when parsing xml
    cout << "error: parsing xml fail" << endl;
    response = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<error>Illegal "
               "XML Format</error>\n";
  }
  else if(request_doc.child("create")){
    int creat_res;
    creat_res = process_create(request_doc, response,C);
    if(creat_res == 0)
    {
      return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" + response;
    }
    else{
      return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<error>Create "
	     "request fail.</error>\n";
    }
  }
  else if(request_doc.child("transactions")){
    int trans_res;
    trans_res = process_transaction(request_doc, response,C);
    if(trans_res == 0){
      return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" + response;
    }
    else{
      return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<error>Transaction "
	"request fail.</error>\n";
    }
  }
  else{
    response = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<error>Illegal "
              "XML Tag</error>\n";
  }
  return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<error>wierd "
         "format XML</error>\n";
}
