#ifndef UI_H
#define UI_H

#include <memory>

#include "glob.h"
#include "connection_initiator_base.h"
#include "connection.h"

class UI;
typedef unique_ptr<UI> UI_ptr;
typedef Connection_base<UI> UI_network;

class UI: Connection_initiator_base {
public:
  UI(){}
  void run();
private:
  void handle_new_connection(tcp::socket socket);
  void init_readline();
  void get_text_input();
  Connection_ptr connection;
};

#endif // UI_H
