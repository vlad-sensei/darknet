#include "message_base.h"

#include <iostream>

Message_base::Message_base(Msg_base_ptr msg){
  type = msg->type;
  h = msg->h;
  encode();
}

void Message_base::update_payload_info() {
  payload_size=0;
  for(const auto& it : h) payload_size+=sizeof(Key_type_t)  + sizeof(Value_size_t) + it.second.size();
  key_num=h.size();
}

void Message_base::encode_to_stream(ba::streambuf &raw, Message_base* msg) {
  msg->update_payload_info();
  ostream os(&raw);
  os.write((char*)&msg->type, sizeof(Msg_type_t));
  os.write((char*)&msg->payload_size, sizeof(Msg_size_t));
  os.write((char*)&msg->key_num, sizeof(Key_num_t));
  Value_size_t value_size;
  for(const auto& it : msg->h) {
    os.write((char*)&it.first, sizeof(Key_type_t));
    value_size = it.second.size();
    os.write((char*)&value_size, sizeof(Value_size_t));
    os.write(it.second.data(), value_size);
  }
}

void Message_base::encode(){
  encode_to_stream(raw,this);
}

//TODO: check for troll input
void Message_base::decode_header() {
  istream is(&raw);
  is.read((char*)&type, sizeof(Msg_type_t));
  is.read((char*)&payload_size, sizeof(Msg_size_t));
  is.read((char*)&key_num, sizeof(Key_num_t));
}

void Message_base::decode_msg() {
  istream is(&raw);
  Key_type_t key;
  Value_size_t value_size;
  for(Key_type_t i=0; i < key_num; i++) {
    is.read((char*)&key, sizeof(Key_type_t));
    is.read((char*)&value_size, sizeof(Value_size_t));
    h[key]=string(value_size,'\0');
    is.read(&h[key][0],value_size);
  }
}

bool Message_base::has_key(const Key_type_t &key){
  return h.find(key)!=h.end();
}

//  ---------- get --------

Message_base::Msg_type_t Message_base::get_type(){
  return type;
}

Message_base::Msg_size_t Message_base::get_payload_size(){
  return payload_size;
}

ba::streambuf& Message_base::get_raw(){
  return raw;
}

template<typename T>
T Message_base::get(const Key_type_t& key){
  T res = T();
  if(!has_key(key)||h[key].size()!=sizeof(T)){
    handle_troll_input();
    return res;
  }
  memcpy(&res, h[key].data(), sizeof(T));
  return res;
}

string&& Message_base::get_string(const Key_type_t& key){
  return move(h[key]);
}

time_t Message_base::get_time_t(const Key_type_t& key){
  return get<time_t>(key);
}

// ------------- put ----------

Msg_base_ptr Message_base::make_base(const Msg_type_t& type){
  Msg_base_ptr res(new Message_base);
  res->type = type;
  res->make_body();
  return res;
}

void Message_base::make_body(){
  encode();
}

template<typename T>
string&& Message_base::to_binary_base(const T& value){
  return string((char*)(&value),sizeof(value));
}

string&& Message_base::to_binary(string&&value){
  return move(value);
}

// ----------- other -----------

void Message_base::handle_troll_input(){
  printf("**** troll input in message!\n");
}
