#include "ui.h"

UI::UI() {
  printf(" ~~~ welcome to darknet ~~~\n");
  thread([this](){get_raw_input();}).detach();
}

void UI::get_raw_input(){
  string raw_input;
  while(getline(cin,raw_input))
    thread([this,raw_input](){
      process_raw_input(move(raw_input));
    }).detach();
}

void UI::process_raw_input(string raw_input){
  cout << "user input: " << raw_input << "\n";
}
