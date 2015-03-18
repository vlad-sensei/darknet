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
 *  ~~ PAYLOAD [variable size]
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
#include <unordered_set>

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

  Message_base(Msg_base_ptr msg);

  void print();
  void decode_header();
  void decode_body();

  Msg_type_t get_type();
  Msg_size_t get_body_size();
  ba::streambuf& get_raw();

  bool has_key(const Key_type_t& key);

protected:
  Message_base(){}

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
  // retrieval

  void update_body_info();
  void encode(); //put msg to streambuf (raw) add body!
  void encode_to_stream(ba::streambuf &raw, Message_base* msg);

  //for malformed messsages
  void handle_troll_input();

  // data
  Msg_type_t type = 0;
  Msg_size_t body_size = 0;
  Key_num_t key_num = 0;
  ba::streambuf raw;
  unordered_map<Key_type_t, string> h;

private:
  template<typename T>
  T get(const Key_type_t& key){
    T res = T();
    if(!has_key(key)||h[key].size()!=sizeof(T)){
      handle_troll_input();
      return res;
    }
    memcpy(&res, h[key].data(), sizeof(T));
    return res;
  }

  template<typename T>
  vector<T> get_vector(const Key_type_t& key){
    const string& bin = h[key];
    size_t size = bin.size()/sizeof(T);
    vector<T> res(size);
    for(size_t i = 0; i<size; i++)
      memcpy(&res[i],&bin[0]+i*sizeof(T),sizeof(T));
    return res;
  }

  template<typename T>
  unordered_set<T> get_unordered_set(const Key_type_t& key){
    const string& bin = h[key];
    size_t size  = bin.size()/sizeof(T);
    unordered_set<T> res;
    T buff;
    for(size_t i=0; i<size; i++){
      memcpy((char*)&buff,&bin[0]+i*sizeof(T), sizeof(T));
      res.emplace(buff);
    }
    return res;
  }



public:
  inline string get_string(const Key_type_t& key){return move(h[key]);}
  inline time_t get_ts_t(const Key_type_t& key){return get<ts_t>(key);}
  inline unsigned get_unsigned(const Key_num_t& key){return get<unsigned>(key);}
  inline bool get_bool(const Key_num_t& key){return get<bool>(key);}
  inline peer_id_t get_peer_id_t(const Key_type_t &key){return get<peer_id_t>(key);}

protected:
  template<typename T>
  const string to_binary_container(const T& container){
    string res(container.size()*sizeof(typename T::value_type),0);
    size_t offset = 0;
    for(const auto& e : container){
      memcpy(&res[0]+offset, &e, sizeof(e));
      offset+=sizeof(e);
    }
    return move(res);
  }

  template<typename T>
  inline string to_binary_base(const T& value){return string((char*)&value,sizeof(T));}
  inline string to_binary(const string& value) {return move(value);}
  inline string to_binary(const time_t& value){return to_binary_base<time_t>(value);}
  inline string to_binary(const bool& value){return to_binary_base<bool>(value);}
  inline string to_binary(const peer_id_t& value){return to_binary_base<peer_id_t>(value);}

};

#endif // MESSAGE_BASE_H
