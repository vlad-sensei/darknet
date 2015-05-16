#include "peer.h"
#include "core.h"

//typedef Network_base<Peer> Peer_network;

Peer::Peer(socket_ptr &sock_, const peer_id_t& pid): Peer_connection(sock_),
  data(pid){
  debug("creating peer [id:%u]..", pid);
  //do_read_header();
}

Peer::~Peer(){
  debug("disconnecting from peer [pid:%d]", data.pid);
}

void Peer::init(){
  Peer_connection::run();
  debug("sending listeing port [%s]",core->get_daemon_port());
  send_listen_port(core->get_daemon_port());
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
    handle_meta_reply(msg);
    break;
  case Message::T_CHUNK:
    handle_chunk(msg);
    break;
  case Message::T_LISTEN_PORT:
    handle_listen_port(msg);
    break;
  case Message::T_CONNECT:
    handle_connect(msg);
    break;
  case Message::T_PEER_REQ:
    handle_peer_req(msg);
    break;
  case Message::T_CHUNK_QUERY:
    handle_chunk_query(msg);
    break;
  case Message::T_CHUNK_ACK:
    handle_chunk_ack(msg);
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
  const Id& bid = msg->get_id(Message::K_ID);
  string data = msg->get_string(Message::K_BODY);
  Chunk chunk(data);
  core->handle_chunk(bid,chunk);
}

void Peer::handle_chunk_ack(const Msg_ptr &msg){
    const Id& bid = msg->get_id(Message::K_ID);
    const unordered_set<Id> cids = msg->get_unordered_set_id(Message::K_CIDS);
    core->handle_chunk_ack(bid,cids,data.pid);


}


void Peer::handle_listen_port(const Msg_ptr &msg){

  const uint16_t& port=msg->get_uint16_t(Message::K_PORT);
  debug("seting listen port [%s]",port);
  set_listen_port(port);
}

void Peer::handle_connect(const Msg_ptr &msg){
  const ip_t& addr=msg->get_ip_t(Message::K_IP);
  const uint16_t& port=msg->get_uint16_t(Message::K_PORT);
  const string addr_str=ba::ip::address_v4(addr).to_string();
  debug("handle connect [%s:%s]",addr_str,port);
  core->connect(addr_str,port);
}

void Peer::handle_peer_req(const Msg_ptr &msg){
  //TODO troll check
  const uint16_t max_count = msg->get_uint16_t(Message::K_PEER_COUNT);
  core->share_peers(max_count,data.pid);
}


void Peer::handle_chunk_req(const Msg_ptr &msg){
  const Id& bid = msg->get_id(Message::K_ID);
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

void Peer::handle_chunk_query(const Msg_ptr &msg){
  const Id& bid = msg->get_id(Message::K_ID);
  const unordered_set<Id> cids = msg->get_unordered_set_id(Message::K_CIDS);
  const bool resend=msg->get_bool(Message::K_RESEND);
  //TODO: do something with them..
  debug("handle_chunk_req");
  //(void)bid; (void)cids;
  if(!resend){
      unordered_set<Id> ack_cids;
      for(Id cid:cids){
        Chunk chunk;
        if(core->get_chunk(bid,cid,chunk)){
          debug("sedning ack for chunk: %s",cid);
          ack_cids.emplace(cid);
        }
      }
      send(Message::chunk_ack(bid,ack_cids));
  }else{
        debug("handle_aggresive_query");
        core->handle_aggresiv_query(bid,cids,data.pid);
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

// ----------------- peer data -----------

void Peer::set_listen_port(const uint16_t& listen_port){
  w_lock l(listen_port_mtx);
  data.listen_port = listen_port;
}

bool Peer::get_listen_port(uint16_t &listen_port){
  r_lock l(listen_port_mtx);
  if(data.listen_port == 0) return false;
  listen_port = data.listen_port;
  return true;
}
