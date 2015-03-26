#include "ui_client.h"

void UI_client::init(){
  Network_base::run();
}

void UI_client::process_msg(const Msg_ptr &msg){
  switch(msg->get_type()){
  case Message::T_UI_TEXT_COMMAND:
    handle_ui_text_command(msg);
    break;
  default:
    debug(" *** invalid message in UI [type:%d]", msg->get_type());
    break;
  }
}

void UI_client::handle_ui_text_command(const Msg_ptr &msg){
  const string& command = msg->get_string(Message::K_TEXT);
  debug("got command : %s", command);
}
