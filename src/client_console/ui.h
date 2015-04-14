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
  UI():is_interactive(true){}
  void run(uint16_t ui_port, string cmd="");
  void echo(const string& msg);
private:
  void handle_new_connection(tcp::socket socket);
  string find_match(string input); //autofill funktion
  void print_terminal_content(vector<string> terminal_content, int content_index);
  void get_text_input();
  void init_window();
  Connection_ptr connection;
  bool is_interactive;
};

#define HANDLE_CMD(cmd_enum, enum_val, cmd_full_name, cmd_short_name) cmd_full_name, cmd_short_name,
static vector<string> command_list {
CMD_LIST
};
#undef HANDLE_CMD

#endif // UI_H
