#include "ui_client.h"

#include "ui.h"

UI_ptr ui;

void UI_client::init(){
  ui = make_unique<UI>();
  Connection_base::run();
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
  string command = msg->get_string(Message::K_TEXT);

  string ret_val = ui->process_text_input(command);

  Msg_ptr ret_msg = Message::echo(ret_val);

  send(ret_msg);
}
