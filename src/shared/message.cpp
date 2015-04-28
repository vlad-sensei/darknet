#include "message.h"

/*
 * use this to make all other messages:
 * Msg_ptr update_values(value1, value2){
 *  return make(T_MSG_TYPE, K_KEY1, value1, K_KEY2, value2);
 * }
 */

Msg_ptr Message::empty(){
  return Msg_ptr(new Message);
}

/*
Msg_ptr Message::copy_msg(Msg_ptr other){
  Msg_ptr res(other);
  return res;
}
*/

Msg_ptr Message::echo(const string& msg){
  return make(T_ECHO, K_BODY, msg);
}

Msg_ptr Message::chunk_req(const Id &bid, const unordered_set<Id> &cids){
  return make(T_CHUNK_REQ, K_BID, bid, K_CIDS, cids);
}

Msg_ptr Message::chunk(const Id& bid,const Chunk& chunk){
  return make(T_CHUNK, K_BID, bid,K_BODY,chunk.data);
}


Msg_ptr Message::ui_text_command(const string& msg){
  return make(T_UI_TEXT_COMMAND, K_BODY, msg);
}

Msg_ptr Message::meta_req(){
  return make(T_META_REQ);
}

Msg_ptr Message::port(uint16_t port){
  return make(T_LISTEN_PORT,K_PORT,port);
}

Msg_ptr Message::merge_peer_req(const ip_t& addr,const uint16_t& port){
  return make(T_CONNECT,K_IP,addr,K_PORT,port);
}

Msg_ptr Message::meta_reply(const vector<Metahead> &metaheads){
  return make(T_META_REPLY, K_META_LIST, metaheads);
}

/*
void f(){
  vector<Metahead> m;
  to_binary_container(m);
}
*/
