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

Msg_ptr Message::chuhk_req(const Id &bid, const unordered_set<Id> &cids){
  return make(T_CHUNK_REQ, K_BID, bid, K_CIDS, cids);
}

Msg_ptr Message::meta_req(){
  return make(T_META_REQ);
}

Msg_ptr Message::meta_reply(const vector<Metahead> &metaheads){
  return make(T_META_REPLY, K_BODY, metaheads);
}

/*
void f(){
  vector<Metahead> m;
  to_binary_container(m);
}
*/
