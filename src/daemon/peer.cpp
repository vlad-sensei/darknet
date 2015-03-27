#include "peer.h"
#include "core.h"

//typedef Network_base<Peer> Peer_network;

Peer::Peer(socket_t &sock_, const peer_id_t& pid): Peer_connection(sock_),
  data(pid) {
  debug("creating peer [id:%u]..", pid);
  //do_read_header();
}

Peer::~Peer(){
  debug("disconnecting from peer [pid:%d]", data.pid);
}

void Peer::init(){
  Peer_connection::run();
}

void Peer::terminate(){
  core->remove_peer(data.pid);
}

// -------------- handlers --------------

void Peer::process_msg(const Msg_ptr& msg){
  debug("processing message..");
  switch(msg->get_type()){
  case Message::T_ECHO:
    handle_echo(msg);
    break;
  case Message::T_CHUNK_REQ:
    handle_chunk_req(msg);
    break;
  case Message::T_META_REQ:
    handle_meta_req(msg);
    break;
  case Message::T_META_LIST:
    handle_meta_list(msg);
    break;
  default:
    debug("unknown messape type");
  }
}

void Peer::handle_echo(const Msg_ptr &msg){
  const string& text = msg->get_string(Message::K_BODY);
  safe_printf("ECHO : %s\n", text);
}

void Peer::handle_chunk_req(const Msg_ptr &msg){
  const Id& bid = msg->get_id(Message::K_BID);
  const unordered_set<Id> cids = msg->get_unordered_set_id(Message::K_CIDS);
  //TODO: do something with them..
  (void)bid; (void)cids;
}

void Peer::handle_meta_req(const Msg_ptr &msg){
  safe_printf("SYNCH_REQ: %s", msg);

}

void Peer::handle_meta_list(const Msg_ptr &msg){
  debug("META_LIST_REPLY: %s", msg);
  //save to inventory
}
