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

string find_match(string input); //autofill funktion
void print_terminal_content(vector<string> terminal_content, int content_index);

char** cmd_list;
vector<char*> command_keys;
vector<string> command_map;


void UI::get_text_input(){

    init_readline();

    vector<string> terminal_content; //all the terminal content to be printed
    vector<string> old_commands; //all old used commands

    //initiate curse window
    initscr();
    noecho();    
    cbreak();  //disables input buffering
    keypad(stdscr, TRUE); //allow special keys like arrow up etc

    string line("");
    size_t c,x,y,max_x,max_y,history_index, content_index = 0;

    terminal_content.push_back(" ~~~ welcome to darknet ~~~ ");
    terminal_content.push_back("> ");
    terminal_content.push_back("");

    printw(terminal_content[0].c_str(),0);
    move(1,0);
    printw(terminal_content[1].c_str(),1);

    while(1){

        c = 0;
        c = wgetch(stdscr); // Wait for user input
        getyx(stdscr,y,x);
        getmaxyx(stdscr,max_y,max_x);

        line.clear();
        line = terminal_content[1];

        switch(c){        
        case KEY_BACKSPACE:
            if(x > 2){
                terminal_content[1].pop_back();
                move(y,x-1);
                delch();
            }
            break;
        case '\n'://ENTER            
            if(line.size()>0) old_commands.insert(old_commands.begin(),terminal_content[1]);
            terminal_content.insert(terminal_content.begin()+1,"> ");
            break;
        case KEY_UP:            
            if(history_index == old_commands.size()){
                terminal_content[1] = "> ";
                history_index = 0;
            }
            else{
                terminal_content[1] = old_commands[history_index];
                history_index++;
            }           
            break;
        case KEY_DOWN:
            if(history_index > 0 ){
                history_index--;
                terminal_content[1] = old_commands[history_index];
            }
            break;

        case KEY_PPAGE://page up           
            if(content_index > 0)content_index--;
            break;
        case KEY_NPAGE://page down
            if(max_y < terminal_content.size() && content_index < terminal_content.size() - max_y)content_index++;
            break;

        case '\t':            
            terminal_content[1] = find_match(terminal_content[1]);
            break;

        case KEY_RESIZE: //prevents resize character from being printed in terminal
            break;
        default:
            terminal_content[1] += c;
        }

        //reset the history index if c not arrow up/down
        if(c != KEY_UP && c != KEY_DOWN)history_index = 0;

        //clear print area and print new text
        print_terminal_content(terminal_content, content_index);

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
    string temp = input.substr(2);

    for(string str:command_map){
        if(temp.size() > 0 && str.find(temp) == 0) return ("> ")+str;
    }
    return input;
}



void print_terminal_content(vector<string>terminal_content, int content_index){
    size_t x,y,max_x,max_y =0;
    string line = "";

    getyx(stdscr,y,x);
    getmaxyx(stdscr,max_y,max_x);

    erase();        //clear entire terminal

    size_t index = 0;
    size_t i = 0;
    bool scroll_set = FALSE;

    while(index < max_y){

        if(i >= terminal_content.size()-1)break;

        if(i > 1 && !scroll_set){
            i += content_index;
            scroll_set = TRUE;
        }
        if(line.size() < 1) line = terminal_content[i];  // 1 line per content row

        else if(line.size() < max_x){
            printw(line.c_str(),index);
            line.clear();
            i++;
            index++;
            move(index,0);
        }
        else if(line.size() >= max_x){
            string temp = line.substr(0,max_x);
            printw(temp.c_str(),index);
            line = line.substr(max_x);
            if(line.size() < 1)i++;
            index++;
            move(index,0);
        }

    }
    // move cursor to correct position
    int temp1 = 1+terminal_content[1].size()/max_x;
    int temp2 = terminal_content[1].size() % max_x;
    move(temp1,temp2);
    refresh();
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

