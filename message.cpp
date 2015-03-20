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
  return make(T_ECHO, K_TEXT, msg);
}

Msg_ptr Message::verify(const peer_id_t &pid){
  return make(T_VERIFY, K_PEER_ID, pid);
}

Msg_ptr Message::chunk_req(const peer_id_t &pid){
    return make(T_CHUNK_REQ, K_PEER_ID, pid);
}
