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

Msg_ptr Message::chunk_query(const Id &bid, const unordered_set<Id> &cids, const bool &resend){
  return make(T_CHUNK_QUERY, K_BID,  bid, K_CIDS, cids, K_RESEND, (bool_t)resend);
}

Msg_ptr Message::chunk_ack(const Id &bid, const unordered_set<Id> &cids){
  return make(T_CHUNK_ACK, K_BID,  bid, K_CIDS, cids);
}

Msg_ptr Message::chunk_req(const Id &bid, const unordered_set<Id> &cids){
  return make(T_CHUNK_REQ, K_ID, bid, K_CIDS, cids);
}

Msg_ptr Message::chunk(const Id& bid,const Chunk& chunk){
  return make(T_CHUNK, K_ID, bid,K_BODY,chunk.data);
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

Msg_ptr Message::adress(const ip_t& addr,const uint16_t& port){
  return make(T_CONNECT,K_IP,addr,K_PORT,port);
}

Msg_ptr Message::meta_list(const vector<Metahead> &metaheads){
  return make(T_META_LIST, K_META_LIST, metaheads);
}

Msg_ptr Message::peer_req(const uint16_t& count){
  return make(T_PEER_REQ, K_PEER_COUNT, count);
}

Msg_ptr Message::assemble(const string& filename,const Result_code& res){
  return make(T_ASSEMBLE, K_STRING, filename,K_RESULT,res);
}

Msg_ptr Message::upload(const Id& mid, const Result_code &res){
  return make(T_UPLOAD, K_ID, mid,K_RESULT, res);
}

Msg_ptr Message::download(const Id& bid, const Result_code &res){
  return make(T_DOWNLOAD, K_ID, bid,K_RESULT, res);
}

Msg_ptr Message::broadcast(const string& msg,const Result_code& res){
  return make(T_BROADCAST, K_STRING, msg, K_RESULT, res);
}

Msg_ptr Message::synch(const Result_code& res){
  return make(T_SYNCH, K_RESULT, res);
}

Msg_ptr Message::search(const vector<Metahead> &metaheads, const Result_code& res){
  return make(T_SEARCH, K_META_LIST, metaheads, K_RESULT, res);
}

Msg_ptr Message::connect(const string& addr, const uint16_t& port, const Result_code& res){
  return make(T_CONNECT, K_STRING, addr, K_PORT, port, K_RESULT, res);
}

Msg_ptr Message::merge(const Result_code& res){
  return make(T_MERGE, K_RESULT, res);
}

Msg_ptr Message::req_peers(const Result_code& res){
  return make(T_REQ_PEERS, K_RESULT, res);
}
