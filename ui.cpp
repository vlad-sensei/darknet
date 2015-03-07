#include "ui.h"

UI::UI() {
  init_commands();
  printf(" ~~~ welcome to darknet ~~~\n");
  thread([this](){get_text_input();}).detach();
}

void UI::get_text_input(){
  string text_input;
  while(getline(cin,text_input)){
    thread([this,text_input](){
       process_text_input(move(text_input));
    }).detach();
  }
}

void UI::process_text_input(string text_input){
  stringstream ss(text_input);
  string command;
  if(!(ss>>command)){
    printf(">");
    return;
  }
  if(command=="1"){ //for test
    connect("localhost", LISTEN_PORT);
    return;
  }
  if(command=="2"){
    string msg = ss.str();
    broadcast_echo(msg);
  }
  if(command=="connect"){
    string peer;
    if(!(ss>>peer)) return;
    uint16_t port = LISTEN_PORT;
    ss >> port;
    connect(peer,port);
    return;
  }
}

void UI::Command::exec(const vector<string> &args){
  if(args.size()<minargc_||args.size()>maxargc_){
    printf("Wrong argument count\n");
    help();
    return;
  }
  try{
    execute_(args);
  } catch(exception&e){
    printf(" *** error processing command : %s\n",e.what());
  }
}

void UI::init_commands(){


}

