#ifndef MESSAGE_BASE_H
#define MESSAGE_BASE_H

/*
 * Message_base
 *
 * Class represataion of message object sent between
 * peers. Contains header with indication of message type,
 * payload size and number of keys in the payload. Each
 * key-value(payload) pair has a key, size of the value
 * in bytes in the value.
 *
 * This is a base class. Derived class should call make()
 * in order to construct a message object.
 *
 * Message [variable size]
 * +------------+-------------------------+
 * | HEADER [8] | PAYLOAD [variable size] |
 * +------------+-------------------------+
 *
 *  ~~ HEADER [8]
 * +--------------+--------------+-------------+
 * | MSG_TYPE [1] | MSG_SIZE [4] | KEY_NUM [1] |
 * +--------------+--------------+-------------+
 *
 *  ~~ PALOAD [variable size]
 * +---------------------+----------------+-----------------------+
 * | KEY_1_TYPE [1]      | VALUE_SIZE [4] | VALUE [variable size] |
 * +---------------------+----------------+-----------------------+
 * | KEY_2_TYPE [1]      | VALUE_SIZE [4] | VALUE [variable size] |
 * +---------------------+----------------+-----------------------+
 * |                               ...                            |
 * +---------------------+----------------+-----------------------+
 * | KEY_KEYNUM_TYPE [1] | VALUE_SIZE [4] | VALUE [variable size] |
 * +---------------------+----------------+-----------------------+
 *
 * Message class also handles encoding and decoding of the message
 */

#include <string>
#include <unordered_map>

#include <unordered_map>
#include <boost/asio.hpp>

namespace ba = boost::asio;

#include "glob.h"

class Message_base;
typedef shared_ptr<Message_base> Msg_base_ptr;

class Message_base
{
public:
  typedef uint8_t Msg_type_t;
  typedef uint32_t Msg_size_t;
  typedef uint8_t Key_num_t;
  typedef uint8_t Key_type_t;
  typedef uint32_t Value_size_t;

  static const size_t HEADER_SIZE = sizeof(Msg_type_t) + sizeof(Msg_size_t) + sizeof(Key_num_t);

  Message_base(){}
  Message_base(Msg_base_ptr msg);

  enum msg_types : Msg_type_t {T_NONE, T_ECHO}; // message types
  enum msg_keys : Key_type_t {}; // header keys


  void print();
  void decode_header();
  void decode_msg();

  Msg_type_t get_type();
  Msg_size_t get_payload_size();
  ba::streambuf& get_raw();

  bool has_key(const Key_type_t& key);

  string&& get_string(const Key_type_t& key);
  time_t get_time_t(const Key_type_t& key);

protected:

  // contructor related
  template<typename ...Ts>
  inline static Msg_base_ptr make_base(const Msg_type_t& type,const Ts&... args){
    Msg_base_ptr res(new Message_base);
    res->type = type;
    res->make_body(args...);
    return res;
  }

  static Msg_base_ptr make_base(const Msg_type_t& type);

  template<typename T, typename ...Ts>
  inline void make_body(const Key_type_t& key, const T& value, const Ts&...args){
    h[key]=to_binary(value);
    make_body(args...);
  }

  void make_body();

  template<typename T>
  string&& to_binary_base(const T& value);
  string&& to_binary(string&& value);

  template<typename T>
  const string to_binary_container(const T& container){
    string res(container.size()*sizeof(typename T::value_type),0);
    size_t offset = 0;
    for(const auto& e : container){
      memcpy(&res[0]+offset, &e, sizeof(e));
      offset+=sizeof(e);
    }
    return res;
  }

  // retrieval
  template<typename T>
  T get(const Key_type_t& key);

  void update_payload_info();
  void encode(); //put msg to streambuf (raw) add body!
  void encode_to_stream(ba::streambuf &raw, Message_base* msg);

  //for malformed messsages
  void handle_troll_input();

  // data
  Msg_type_t type = T_NONE;
  Msg_size_t payload_size = 0;
  Key_num_t key_num = 0;
  ba::streambuf raw;
  unordered_map<Key_type_t, string> h;
};

#endif // MESSAGE_BASE_H
