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
  case Message::T_MID:
    handle_mid(msg);
    break;
  case Message::T_BID:
    handle_bid(msg);
    break;
  case Message::T_ASSEMBLE_FILENAME:
    handle_assemble_filename(msg);
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

#ifdef TEST
  string search_results = "";
  for(auto head: v){
    search_results += "[mid "+head.mid.to_string()+"][tags "+head.tags+"][bid "+head.bid.to_string()+"]\n";
  }
  // Remove trailing newline
  search_results = search_results.substr(0,search_results.length()-1);
  string echo_str = "(<>)\n"+search_results;
  ui->echo(echo_str);
#else
  ui->echo("=== Begin search results ===");
  for (auto e: v){
    ui->echo("Metahead:");
    ui->echo("Mid[" + e.mid.to_string() + "]");
    ui->echo("Bid[" + e.bid.to_string() + "]");
    ui->echo("Tags[" + e.tags + "]");
  }
  ui->echo("=== End search results =");
#endif
}


void Connection::handle_mid(const Msg_ptr &msg){
  const Id& mid = msg->get_id(Message::K_MID);
#ifdef TEST
  ui->echo("(<>) "+mid.to_string());
#else
  ui->echo("Upload successful: Mid["+mid.to_string()+"]");
#endif
}

void Connection::handle_bid(const Msg_ptr &msg){
  const Id& bid = msg->get_id(Message::K_BID);
#ifdef TEST
  ui->echo("(<>) "+bid.to_string());
#else
  ui->echo("Download successful: Bid["+bid.to_string()+"]");
#endif
}

void Connection::handle_assemble_filename(const Msg_ptr &msg){
  const string& filename = msg->get_string(Message::K_FILENAME);
#ifdef TEST
  ui->echo("(<>) "+filename);
#else
  ui->echo("Assembly complete. Filename["+filename+"]");
#endif
}
