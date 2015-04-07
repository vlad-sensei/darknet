#ifndef UI_CLIENT_H
#define UI_CLIENT_H

#include <memory>

#include "glob.h"
#include "connection_base.h"
#include "message.h"

class UI_client;
typedef shared_ptr<UI_client> UI_client_ptr;
typedef Connection_base<UI_client> UI_client_network;

class UI_client : public enable_shared_from_this<UI_client>, UI_client_network {
  UI_client();
public:
  UI_client(tcp::socket& socket):UI_client_network(socket){}
  void init();
private:
  void process_msg(const Msg_ptr& msg);
  void handle_ui_text_command(const Msg_ptr& msg);
  UI_client_ptr shared_from_this() {return enable_shared_from_this<UI_client>::shared_from_this();}
};

#endif // UI_CLIENT_H
