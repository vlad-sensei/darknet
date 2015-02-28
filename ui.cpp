#include "ui.h"
#include "core.h"

UI::UI() {
  printf(" ~~~ welcome to darknet ~~~\n");
  thread([this](){get_raw_input();}).detach();
}

void UI::get_raw_input(){
  string raw_input;
  printf("\n>");
  while(getline(cin,raw_input)){
    thread([this,raw_input](){
      try{
       process_raw_input(move(raw_input));
      } catch(std::exception&e){
        debug("*** troll user put? : %s",e.what());
      }
    }).detach();
  }
}

void UI::process_raw_input(string raw_input){
  stringstream ss(raw_input);
  string command;
  if(!(ss>>command)){
    printf(">");
    return;
  }
  if(command=="1"){ //for test
    core->connect("localhost", LISTEN_PORT);
    return;
  }
  if(command=="2"){
    string msg = ss.str();
    core->broadcast_echo(msg);
  }
  if(command=="connect"){
    string peer;
    if(!(ss>>peer)) return;
    uint16_t port = LISTEN_PORT;
    ss >> port;
    core->connect(peer,port);
    return;
  }
}
