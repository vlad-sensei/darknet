#include "message_base.h"

Message_base::Message_base(Msg_base_ptr msg){
  type = msg->type;
  h = msg->h;
  encode();
}

void Message_base::update_body_info() {
  body_size=0;
  for(const auto& it : h) body_size+=sizeof(Key_type_t)  + sizeof(Value_size_t) + it.second.size();
  key_num=h.size();
}

void Message_base::encode_to_stream(ba::streambuf &raw, Message_base* msg) {
  msg->update_body_info();
  ostream os(&raw);
  os.write((char*)&msg->type, sizeof(Msg_type_t));
  os.write((char*)&msg->body_size, sizeof(Msg_size_t));
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
  is.read((char*)&body_size, sizeof(Msg_size_t));
  is.read((char*)&key_num, sizeof(Key_num_t));
}

void Message_base::decode_body() {
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

Message_base::Msg_size_t Message_base::get_body_size(){
  return body_size;
}

ba::streambuf& Message_base::get_raw(){
  return raw;
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

// ----------- other -----------

void Message_base::handle_troll_input(){
  debug("**** troll input in message!\n");
}

void Message_base::print(){
  debug("message [type:%u;body_size:%llu;keynum:%u]",type,body_size,key_num);
}

// ----------- Binary conversion --------------

/* Serialization format of metahead:
 *
 * +----------+----------+------------+
 * | MID [64] | BID [64] | TAGS [896] |
 * +----------+----------+------------+
 * 0        63|64     127|128        1024
 */
string Message_base::to_binary(const Metahead& metahead){
  const size_t BID_OFFSET = MID_SIZE;
  const size_t TAGS_OFFSET = BID_OFFSET + BID_SIZE;
  const size_t tags_size = metahead.tags.size() >= TAGS_SIZE ? TAGS_SIZE : metahead.tags.size();
  string buffer(METAHEAD_SIZE, 0);

  memcpy(&buffer[0],           &metahead.mid,     MID_SIZE);
  memcpy(&buffer[BID_OFFSET],  &metahead.bid,     BID_SIZE);
  memcpy(&buffer[TAGS_OFFSET], &metahead.tags[0], tags_size);

  return buffer;
}
