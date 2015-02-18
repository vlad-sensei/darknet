#include "message.h"

/*
 * use this to make all other messages:
 * Msg_ptr update_values(value1, value2){
 *  return make(T_MSG_TYPE, K_KEY1, value1, K_KEY2, value2);
 * }
 */

template<typename ...Ts>
Msg_ptr Message::make(Ts&&...args){
  return static_pointer_cast<Message>(make_base(args...));
}

Msg_ptr Message::echo(){
  return make(T_ECHO);
}
