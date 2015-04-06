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

string find_match(string input);

char** cmd_list;
vector<char*> command_keys;
vector<string> command_map;


void UI::get_text_input(){

    init_readline();

    vector<string> old_commands;
    //initiate curse window
    initscr();
    noecho();    
    cbreak();
    keypad(stdscr, TRUE);

    string line("");    
    int c,x,y,index = 0;    

    printw(" ~~~ welcome to darknet ~~~\n");
    move(y+1,0);


    while(1){

        c = 0;
        c = wgetch(stdscr); // Wait for user input
        getyx(stdscr,y,x);

        switch(c){

        case KEY_BACKSPACE:
            if(x > 0){
                line.pop_back();
                move(y,x-1);
                delch();
            }
            break;
        case '\n'://ENTER
            if(line.size()>0) old_commands.insert(old_commands.begin(),line);
            break;
        case KEY_UP:
            line.clear();
            if(index == old_commands.size()){
                line = "";
                index = 0;
            }
            else{
                line = old_commands[index];
                index++;
            }           
            break;
        case KEY_DOWN:
            if(index > 0 ){
                line.clear();
                index--;
                line = old_commands[index];
            }
            break;
        case '\t':            
            line = find_match(line);
            break;
        default:   
            line += c;
        }

        const char* temp;

        //clear print area and print new text
        move(y,0);
        clrtoeol();
        temp = (line).c_str();
        printw(temp);

        //special case for enter
        if(c == '\n'){
            line.clear();
            move(y+1,0);
        }

        //reset the history index if c not arrow up
        if(c != KEY_UP && c != KEY_DOWN)index = 0;

        refresh();


        //connection->text_command(line);
        /*
      add_history(line);
      string input(line);
      connection->text_command(input);
        */

    }

    endwin();
}


string find_match(string input){
    for(string str:command_map){
        if(str.size() > str.find(input) && str.find(input) == 0) return str;
    }
    return input;
}


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

