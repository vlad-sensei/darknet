#ifndef UI_H
#define UI_H

#include <memory>

#include "glob.h"
#include "common.h"
#include "connection_initiator_base.h"
#include "connection.h"

class UI;
typedef unique_ptr<UI> UI_ptr;
typedef Connection_base<UI> UI_network;
class Connection;
typedef shared_ptr<Connection> Connection_ptr;

extern UI_ptr ui; //might need to rename due to overload

class UI: Connection_initiator_base {
public:
  UI(){}
  void run(uint16_t ui_port);
  void echo(const string& msg);
private:
  void handle_new_connection(tcp::socket socket);

#ifdef NCURSES
  string find_match(const string& input); //autofill funktion
  void print_terminal_content(const vector<string>& terminal_content, int content_index);
  void init_window();
#endif

  void get_text_input();
  Connection_ptr connection;
};

#define HANDLE_CMD(cmd_enum, enum_val, cmd_full_name, cmd_short_name) cmd_full_name, cmd_short_name,
static vector<string> command_list {
CMD_LIST
};
#undef HANDLE_CMD

#endif // UI_H
