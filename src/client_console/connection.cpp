#include "connection.h"

void Connection::init(){
  Connection_base::run();
}

void Connection::process_msg(const Msg_ptr &msg){
  switch(msg->get_type()){
  case Message::T_ECHO:
    handle_echo(msg);
    break;
  case Message::T_SEARCH:
    handle_search(msg);
    break;
  case Message::T_CONNECT:
    handle_connect(msg);
    break;
  case Message::T_BROADCAST:
    handle_broadcast(msg);
    break;
  case Message::T_UPLOAD:
    handle_upload(msg);
    break;
  case Message::T_DOWNLOAD:
    handle_download(msg);
    break;
  case Message::T_ASSEMBLE:
    handle_assemble(msg);
    break;
  case Message::T_SYNCH:
    handle_synch(msg);
    break;
  case Message::T_MERGE:
    handle_merge(msg);
    break;
  case Message::T_REQ_PEERS:
    handle_req_peers(msg);
    break;
  default:
    break;
  }
  cv.notify_one();
}

void Connection::handle_echo(const Msg_ptr &msg){
  const string& echo_str = msg->get_string(Message::K_STRING);
  ui->echo(echo_str);
}

void Connection::handle_search(const Msg_ptr &msg){
  Result_code res = msg->get_result(Message::K_RESULT);
  debug("Result code [%s] of search",res);

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

void Connection::handle_upload(const Msg_ptr &msg){
  Result_code res = msg->get_result(Message::K_RESULT);
  debug("Result code [%s] of upload",res);

  const Id& mid = msg->get_id(Message::K_ID);
#ifdef TEST
  ui->echo("(<>) "+mid.to_string());
#else
  ui->echo("Upload successful: Mid["+mid.to_string()+"]");
#endif
}

void Connection::handle_download(const Msg_ptr &msg){
  const Id& bid = msg->get_id(Message::K_ID);
#ifdef TEST
  ui->echo("(<>) "+bid.to_string());
#else
  ui->echo("Download successful: Bid["+bid.to_string()+"]");
#endif
}

void Connection::handle_assemble(const Msg_ptr &msg){
  const string& filename = msg->get_string(Message::K_STRING);
#ifdef TEST
  ui->echo("(<>) "+filename);
#else
  ui->echo("Assembly complete. Filename["+filename+"]");
#endif
}

void Connection::handle_broadcast(const Msg_ptr &msg){
  // Broadcast message contains the broadcasted message.
  // Should already be known by the client, but fetch and print anyway
  const string& str = msg->get_string(Message::K_STRING);

#ifdef TEST
  ui->echo("(<>) "+str);
#else
  ui->echo("Broadcasted message ["+str+"]");
#endif
}

void Connection::handle_connect(const Msg_ptr &msg){
  const uint16_t& port = msg->get_uint16_t(Message::K_PORT);
  const string& addr = msg->get_string(Message::K_STRING);
#ifdef TEST
  ui->echo("(<>) Connected to '"+addr+":"+to_string(port)+"'.");
#else
  ui->echo("Connected to '"+addr+":"+to_string(port)+"'.");
#endif
}

void Connection::handle_synch(const Msg_ptr &msg){
  Result_code res = msg->get_result(Message::K_RESULT);
  debug("Result code [%s] of synch",res);

#ifdef TEST
  ui->echo("(<>) Synch started");
#else
  ui->echo("Synch started.");
#endif
}

void Connection::handle_merge(const Msg_ptr &msg){
  Result_code res = msg->get_result(Message::K_RESULT);
  debug("Result code [%s] of merge",res);

#ifdef TEST
  ui->echo("(<>) Merging");
#else
  ui->echo("Merging...");
#endif
}

void Connection::handle_req_peers(const Msg_ptr &msg){
  Result_code res = msg->get_result(Message::K_RESULT);
  debug("Result code [%s] of req_peers",res);

#ifdef TEST
  ui->echo("(<>) Requesting");
#else
  ui->echo("Requesting..");
#endif
}
