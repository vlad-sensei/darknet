#ifndef MESSAGE_H
#define MESSAGE_H

/*
 * Message
 *
 * Message class with its static contructors
 */

#include <memory>

#include "glob.h"
#include "common.h"
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
    T_META_REPLY, //message contains a list of metaheads
    T_CHUNK,
    T_UI_TEXT_COMMAND,
    T_LISTEN_PORT,
    T_CONNECT
  }; // message types
  enum msg_keys : Key_type_t {
    K_BODY,
    K_BID,
    K_CIDS,
    K_META_LIST,
    K_PORT,
    K_IP
  }; // header keys

  static Msg_ptr empty();
  //static Msg_ptr copy_msg(Msg_ptr other);
  static Msg_ptr echo(const string& msg = "");
  static Msg_ptr chunk_req(const Id& bid, const unordered_set<Id>& cids);
  static Msg_ptr ui_text_command(const string& msg = "");
  static Msg_ptr meta_req();
  static Msg_ptr port(uint16_t port);
  static Msg_ptr meta_reply(const vector<Metahead> &meta_list);
  static Msg_ptr chunk(const Id& bid,const Chunk& chunk);
  static Msg_ptr merge_peer_req(const ip_t &addr, const uint16_t &);
};

#endif // MESSAGE_H
