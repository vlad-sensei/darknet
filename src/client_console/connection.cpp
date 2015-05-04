#include "connection.h"

void Connection::init(){
  Connection_base::run();
}

void Connection::process_msg(const Msg_ptr &msg){
  switch(msg->get_type()){
  case Message::T_ECHO:
    handle_echo(msg);
    break;
  case Message::T_META_LIST:
    handle_meta_list(msg);
    break;
  default:
    break;
  }
  cv.notify_one();
}

void Connection::handle_echo(const Msg_ptr &msg){
  const string& echo_str = msg->get_string(Message::K_BODY);
  ui->echo(echo_str);
}

void Connection::handle_meta_list(const Msg_ptr &msg){
  vector<Metahead> v = msg->get_vector_metahead(Message::K_META_LIST);

  ui->echo("=== Begin search results ===");
  for (auto e: v){
    ui->echo("Metahead:");
    ui->echo("Mid[" + e.mid.to_string() + "]");
    ui->echo("Bid[" + e.bid.to_string() + "]");
    ui->echo("Tags[" + e.tags + "]");
  }
  ui->echo("=== End search results =");
}
