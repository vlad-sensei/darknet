#ifndef MESSAGE_H
#define MESSAGE_H

/*
 * Message
 *
 * Message class with its static contructors
 */

#include <memory>

#include "glob.h"
#include "message_base.h"

class Message;
typedef shared_ptr<Message> Msg_ptr;

class Message : public Message_base {
public:
  static Msg_ptr copy_msg(Msg_ptr other);
  static Msg_ptr echo(const string& msg = "");
  static Msg_ptr verify(const peer_id_t& pid);
private:
  template<typename ...Ts>
  static Msg_ptr make(const Ts&...args);
};

#endif // MESSAGE_H
