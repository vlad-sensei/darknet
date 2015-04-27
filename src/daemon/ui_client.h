#ifndef Ui_CLIENT_H
#define Ui_CLIENT_H

#include <memory>

#include "glob.h"
#include "connection_base.h"
#include "message.h"

class Ui_client;
typedef shared_ptr<Ui_client> Ui_client_ptr;
typedef Connection_base<Ui_client> Ui_client_network;

class Ui_client : public enable_shared_from_this<Ui_client>, Ui_client_network {
  Ui_client();
public:
  Ui_client(socket_ptr& socket):Ui_client_network(socket){}
  void init();
private:
  void process_msg(const Msg_ptr& msg);
  void handle_ui_text_command(const Msg_ptr& msg);
  Ui_client_ptr shared_from_this() {return enable_shared_from_this<Ui_client>::shared_from_this();}
};

#endif // Ui_CLIENT_H
