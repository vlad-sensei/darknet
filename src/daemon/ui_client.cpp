#include "ui_client.h"
#include "ui.h"

Ui_ptr ui;

void Ui_client::init(){
  ui = unique_ptr<Ui>(new Ui);
  Connection_base::run();
}

void Ui_client::process_msg(const Msg_ptr &msg){
  switch(msg->get_type()){
  case Message::T_UI_TEXT_COMMAND:
    handle_ui_text_command(msg);
    break;
  default:
    debug(" *** invalid message in Ui [type:%d]", msg->get_type());
    break;
  }
}

//should send asynchronously, not wait for processing
void Ui_client::handle_ui_text_command(const Msg_ptr &msg){
  const string& command = msg->get_string(Message::K_BODY);
  debug("command : %s", command);
  const Msg_ptr& ret_val = ui->process_text_input(command);
  send(ret_val);
}
