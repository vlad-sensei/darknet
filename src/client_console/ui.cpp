#include "ui.h"

#include <thread>
#include <mutex>
#include <ncurses.h>
#include <boost/algorithm/string.hpp>

UI_ptr ui;

vector<string> terminal_content; //all the terminal content to be printed
vector<string> old_commands; //all old used commands
uint32_t content_index = 0;


void UI::run(uint16_t ui_port){
  Connection_initiator_base::connect("localhost",ui_port);

  thread network_thread([this](){
    Connection_initiator_base::run();
  });

  thread input_thread([this](){
    get_text_input();
  });

  input_thread.join();

  network_thread.join();
}

void UI::handle_new_connection(tcp::socket socket){
  connection = Connection_ptr(new Connection(socket));
  connection->init();
}


void UI::echo(const string &msg){

#ifdef NCURSES

  vector <string> strings;

  boost::split(strings,msg,boost::is_any_of("\n"));

  for(string str: strings){
    terminal_content.insert(terminal_content.begin()+2,str);
  }
  print_terminal_content(terminal_content, content_index);
  refresh();

#else
  safe_printf("%s\n",msg);
#endif

}

// ~~~~~~~~~~~~ text input ~~~~~~~~~~~~

void UI::get_text_input(){

#ifdef NCURSES

  init_window();
  string line("");
  uint32_t c,x,y,max_x,max_y,history_index = 0;

  while(1){
    c = 0;
    c = wgetch(stdscr); // Wait for user input
    getyx(stdscr,y,x);
    getmaxyx(stdscr,max_y,max_x);
    line.clear();
    line = terminal_content[1];
    string cmd = "";
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
      cmd = line.substr(2);
      if(!cmd.empty()){
        connection->text_command(cmd);
        //wait for return message of sent command
        unique_lock<mutex> lk(connection->m);
        connection->cv.wait(lk);
      }
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

  }
  endwin();

#else
  //Test and Windows prompt

  string cmd="";

  while(true){
    getline(cin,cmd);

    if(!cmd.empty()){
      connection->text_command(cmd);
      //wait for return message of sent command
      unique_lock<mutex> lk(connection->m);
      connection->cv.wait(lk);
    }
  }

#endif

}

#ifdef NCURSES
string UI::find_match(string input){
  string temp = input.substr(2);
  for(string str:command_list){
    if(temp.size() > 0 && str.find(temp) == 0) return "> "+str+" ";
  }
  return input;
}

void UI::print_terminal_content(vector<string>terminal_content, int content_index){
  uint32_t x,y,max_x,max_y,row=0,terminal_index = 0;
  string line = "";
  getyx(stdscr,y,x);
  getmaxyx(stdscr,max_y,max_x);
  erase(); //clear entire terminal
  bool scroll_set = FALSE;
  while(row < max_y){
    if(terminal_index >= terminal_content.size()-1)break;
    if(terminal_index > 1 && !scroll_set){
      terminal_index += content_index;
      scroll_set = TRUE;
    }
    if(line.size() < 1) line = terminal_content[terminal_index]; // 1 line per content row
    else if(line.size() < max_x){
      printw(line.c_str(),row);
      line.clear();
      terminal_index++;
      row++;
      move(row,0);
    }
    else if(line.size() >= max_x){
      string temp = line.substr(0,max_x);
      printw(temp.c_str(),row);
      line = line.substr(max_x);
      if(line.size() < 1)terminal_index++;
      row++;
      move(row,0);
    }
  }
  // move cursor to correct position
  int temp1 = 1+terminal_content[1].size()/max_x;
  int temp2 = terminal_content[1].size() % max_x;
  move(temp1,temp2);
  refresh();

  (void)y;
}


void UI::init_window(){
  //initiate curse window
  initscr();
  noecho();
  cbreak(); //disables input buffering
  keypad(stdscr, TRUE); //allow special keys like arrow up etc

  terminal_content.push_back(" ~~~ welcome to darknet ~~~ ");
  terminal_content.push_back("> ");
  terminal_content.push_back("");
  printw(terminal_content[0].c_str(),0);
  move(1,0);
  printw(terminal_content[1].c_str(),1);
}

#endif
