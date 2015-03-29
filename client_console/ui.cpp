#include "ui.h"

#include <thread>
#include <mutex>
#include <ncurses.h>


UI_ptr ui;


void UI::run(){

  //Connection_initiator_base::connect("localhost",DEFAULT_UI_LISTEN_PORT);

  thread input_thread([this](){
    get_text_input();
  });

  thread network_thread([this](){
    Connection_initiator_base::run();
  });
  network_thread.join();
  input_thread.join();

}

void UI::handle_new_connection(tcp::socket socket){
  connection = Connection_ptr(new Connection(socket));
  connection->init();
}

void UI::echo(const string &msg){
  safe_printf("%s\n", msg);
}

// ~~~~~~~~~~~~ text input ~~~~~~~~~~~~

void UI::get_text_input(){

  init_readline();

  safe_printf(" ~~~ welcome to darknet ~~~\n");

  //----------ncurses parts------------
  initscr();
  raw();

  int c;
  char* line;

  printw(" ~~~ welcome to darknet ~~~\n");
  while(1){
      c = getch(); /* Wait for user input */
      if(c == '\t'){
          //const char* linec = line;
          printw("GGGGGG");
      }
      else{
          line += c;

      }

      refresh(); /* Print it on to the real screen */
  }
  endwin();




  /*
  while(1){

    //    rl_bind_key('\t',rl_complete);

    char* line = readline("> ");

    if(!line) break;
    if(*line){
      add_history(line);

      string input(line);

      connection->text_command(input);

      free(line);

      /*
      //wait for return message of sent command
      unique_lock<mutex> lk(connection->m);
      connection->cv.wait(lk);
      */

      //handle properly with ncurses
    }else{
      free(line);
    }
  }
  */
}




char** cmd_list;
vector<char*> command_keys;
vector<string> command_map;

void UI::init_readline(){

  //TODO: Fix sharing of available commands. This is for testing.
  command_map.push_back("connect");
  command_map.push_back("broadcast");

  for(const auto& key : command_map) {
    char* a = new char[key.size()+1];
    a[key.size()] = '\0';
    memcpy(a,key.c_str(),key.size());
    command_keys.push_back(a);
  }

  //Might be better way to build 'cmd_list'
  cmd_list = &command_keys[0];

}

