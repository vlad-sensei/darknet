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
  case Message::T_META_REPLY:
    handle_meta_reply(msg);
    break;
  case Message::T_CHUNK:
    handle_chunk(msg);
    break;
  case Message::T_LISTEN_PORT:
    handle_listen_port(msg);
    break;
  default:
    debug("unknown messape type");
  }
}

void Peer::handle_echo(const Msg_ptr &msg){
  const string& text = msg->get_string(Message::K_BODY);
  safe_printf("ECHO : %s\n", text);
}

void Peer::handle_chunk(const Msg_ptr &msg){
  const Id& bid = msg->get_id(Message::K_BID);
  string data = msg->get_string(Message::K_BODY);
  Chunk chunk(data);
  core->handle_chunk(bid,chunk);
}

void Peer::handle_listen_port(const Msg_ptr &msg){
  const uint16_t& port=msg->get_uint16_t(Message::K_PORT);
  set_listen_port(port);
}


void Peer::handle_chunk_req(const Msg_ptr &msg){
  const Id& bid = msg->get_id(Message::K_BID);
  const unordered_set<Id> cids = msg->get_unordered_set_id(Message::K_CIDS);
  //TODO: do something with them..
  debug("handle_chunk_req");
  //(void)bid; (void)cids;
  //TODO: trivial send when requested
  for(Id cid:cids){
    Chunk chunk;
    if(core->get_chunk(bid,cid,chunk)){
      debug("seding chunk:");
      send(Message::chunk(bid,chunk));
    }
  }
}

void Peer::handle_meta_req(const Msg_ptr &msg){
  debug("SYNCH_REQ: %s", msg->get_string(Message::K_BODY));

  for(auto e:core->publish_metaheads()){
    debug("seding [mid %s]\n [tags %s]\n [bid %s]\n",e.mid,e.tags,e.bid);
  }

  send_metaheads(core->publish_metaheads());
}

void Peer::handle_meta_reply(const Msg_ptr &msg){
  debug("META_LIST_REPLY:");
  vector<Metahead> v = msg->get_vector_metahead(Message::K_META_LIST);
   for (const auto& e: v){
       core->add_metahead(e);
     }
}
