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
  Message(){}
  template<typename ...Ts>
  inline static Msg_ptr make(const Ts&...args){
    return static_pointer_cast<Message>(make_base(args...));
  }
public:
  enum msg_types : Msg_type_t {
    T_NONE,
    T_ECHO,
    T_CHUNK_REQ,
    T_META_REQ, //sent by a client requesting meta heads
    T_META_LIST //message contains a list of metaheads
  }; // message types
  enum msg_keys : Key_type_t {
    K_BODY,
    K_BID,
    K_CIDS
  }; // header keys

  static Msg_ptr empty();
  //static Msg_ptr copy_msg(Msg_ptr other);
  static Msg_ptr echo(const string& msg = "");
  static Msg_ptr chuhk_req(const Id& bid, const unordered_set<Id>& cids);
  static Msg_ptr meta_req();
};

#endif // MESSAGE_H
