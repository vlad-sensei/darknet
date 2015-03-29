#include "connection.h"

void Connection::init(){
  Connection_base::run();
}

void Connection::process_msg(const Msg_ptr &msg){
  const string& echo_str = msg->get_string(Message::K_BODY);
  safe_printf("%s\n",echo_str);
  lock_guard<mutex> lk(m);
   cv.notify_one();
}
