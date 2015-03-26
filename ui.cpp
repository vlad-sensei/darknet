//TODO: !!!!!! deprecate readline with something more c++:ish

#include "ui.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <iterator>
#include<istream>
#include<string>
#include<stdio.h>
#include<ncurses.h>

//to install ncurses (after using apt-get):
//http://stackoverflow.com/questions/15466654/qt-creator-and-ncurses-initscr-not-found

char** cmd_list;


UI::UI() {
  init_commands();
  init_readline();

  printf(" ~~~ welcome to darknet ~~~\n");
  thread([this](){get_text_input();}).detach();
}

void UI::get_text_input(){
    initscr();
    raw();
    //noecho();
    int c;
    char* line;
    printw("Hello World !!!\n");	/* Print Hello World		  */
    printw(" ~~~ welcome to darknet ~~~\n");
    while(1){
        c = getch();                  /* Wait for user input */
        if(c == '\t'){
            const char* linec = line;
            printw("GGGGGG");

        }
        else{
            line += c;
        }
        printw(line);
        refresh();                  /* Print it on to the real screen */

    }
    endwin();
  /*
  string text_input;
  while(getline(cin,text_input)){
    thread([this,text_input](){
      process_text_input(move(text_input));
    }).detach();
    safe_printf(">");
    cout.flush();
  }
*/
}

void UI::process_text_input(string text_input){
  istringstream ss(text_input);
  //istream_iterator<string> begin(ss), end;
  //vector<string> args(begin, end;
  istream_iterator<string> begin(ss), end;
  vector<string> cmd_args(begin, end);
  if(cmd_args.empty()){
    return;
  }
  r_lock(commands_mtx);
  if(!data.command_exists(cmd_args[0])){
    safe_printf("No such command: %s\n", cmd_args[0]);
    return;
  }
  data.commands[cmd_args[0]]->exec(cmd_args);
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

//TODO: make sure there are no copies
void UI::init_command(const vector<string> &key_words, const cmd_lambda_t &execute, const string &help_text, const unsigned &minargc, const unsigned &maxargc){
  Command_ptr cmd = Command_ptr(new Command(execute, help_text, minargc, maxargc));
  w_lock w(commands_mtx);
  for(const string& key_word : key_words) data.commands[key_word]=cmd;
}

//TODO: check if that this does not lead to any race condition when "this" (UI/Core) is destroyed
void UI::init_commands(){
  init_command({"c", "connect"},
               [this](const vector<string>& args){
    //TODO: find a better way to parse args, perhaps a template of some kind
    string peer;
    uint16_t port = DEFAULT_LISTEN_PORT;
    try{
      peer = args[1];
      if(args.size()>2) port = stoul(args[2]);
    } catch(exception& e){
      handle_invalid_args(e);
      return;
    }
    connect(peer,port);
  },
  "connect PEER_IP [PORT]",
  2,3);

  init_command({"br", "broadcast"},
               [this](const vector<string>& args){
    string msg;
    try{
      msg = args[1];
    } catch(exception& e){
      handle_invalid_args(e);
      return;
    }
    broadcast_echo(msg);
  },
  "broadcast MESSAGE",
  2,2);

  init_command({"exit", "quit"},
               [this](const vector<string>& args){
    (void)args;
    safe_printf("Exiting darknet...");
    exit(0);
  },
  "'exit' or 'quit'",
  1,1);

}

vector<char*> command_keys;

void UI::init_readline(){
  r_lock(commands_mtx);
  for(auto key : data.commands) {
    string str = key.first;
    char* a = new char[str.size()+1];
    a[str.size()] = '\0';
    memcpy(a,str.c_str(),str.size());
    command_keys.push_back(a);
  }
  //Might be better way to build 'cmd_list'
  cmd_list = &command_keys[0];


}

