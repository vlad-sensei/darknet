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
    T_META_LIST, //message contains a list of metaheads
    T_CHUNK,
    T_UI_TEXT_COMMAND,
    T_LISTEN_PORT,
    T_CONNECT,
    T_PEER_REQ,
    T_UPLOAD,
    T_DOWNLOAD,
    T_ASSEMBLE,
    T_BROADCAST,
    T_SYNCH,
    T_SEARCH,
    T_MERGE,
    T_REQ_PEERS

  }; // message types
  enum msg_keys : Key_type_t {
    K_BODY,
    K_ID,
    K_CIDS,
    K_META_LIST,
    K_PORT,
    K_IP,
    K_PEER_COUNT,
    K_STRING,
    K_RESULT
  }; // header keys

  static Msg_ptr empty();
  //static Msg_ptr copy_msg(Msg_ptr other);
  static Msg_ptr echo(const string& msg = "");
  static Msg_ptr chunk_req(const Id& bid, const unordered_set<Id>& cids);
  static Msg_ptr ui_text_command(const string& msg = "");
  static Msg_ptr meta_req();
  static Msg_ptr port(uint16_t port);
  static Msg_ptr meta_list(const vector<Metahead> &meta_list);
  static Msg_ptr chunk(const Id& bid,const Chunk& chunk);
  static Msg_ptr adress(const ip_t &addr, const uint16_t& port);
  static Msg_ptr peer_req(const uint16_t &);

  static Msg_ptr upload(const Id& mid,const Result_code& res);
  static Msg_ptr download(const Id& bid,const Result_code& res);
  static Msg_ptr assemble(const string& filename,const Result_code& res);
  static Msg_ptr broadcast(const string& msg, const Result_code &res);
  static Msg_ptr connect(const string &addr, const uint16_t& port, const Result_code& res);
  static Msg_ptr synch(const Result_code& res);
  static Msg_ptr search(const vector<Metahead> &metaheads, const Result_code& res);
  static Msg_ptr merge(const Result_code& res);
  static Msg_ptr req_peers(const Result_code& res);

};


#endif // MESSAGE_H
