#include "peer.h"
#include "core.h"

Peer::Peer(socket_t &sock_, const peer_id_t& pid):
  sock(move(sock_)), data(pid) {
  debug("creating peer [id:%llu]..", int64_t(pid));
  do_read_header();
}

Peer::~Peer(){
  debug("disconnecting from peer..\n");
}


// ----------- network ----------

void Peer::send(Msg_ptr msg) {
  debug("sending message..\n");
  unique_lock<mutex> lck(write_msg_mtx);
  msg_queue.push_back(msg);
  lck.unlock();
  do_write();
}

void Peer::do_write() {
  unique_lock<mutex> lck(write_msg_mtx);
  if(msg_queue.empty()|| msg_is_writing) return;

  //decide to write
  //no need to do atomic exchange -- msg_lock in affect
  msg_is_writing = true;
  write_msg = msg_queue.front();
  msg_queue.pop_front();
  lck.unlock();
  ba::async_write(sock,write_msg->get_raw(),
                  [this](const bs::error_code& ec, size_t) {
    unique_lock<mutex> lck(write_msg_mtx);
    write_msg.reset();
    msg_is_writing = false;
    lck.unlock();
    if(ec) {
      handle_connection_error("do_write", ec);
      return;
    }
    debug(" --- succesfully sent ---\n");
    do_write();
  });
}

void Peer::handle_connection_error(const string &location, const boost::system::error_code &ec){
  cerr << location << " : " << ec.message() << "\n";
  core->remove_peer(data.pid);
}

void Peer::do_read_header() {
  debug("reading new message..\n");
  read_msg.reset(new Message);
  ba::async_read(sock,read_msg->get_raw(),
                 [this](const bs::error_code& ec, size_t bytes) {
    if(ec) return (size_t)0;
    return Message::HEADER_SIZE-bytes;
  },
  [this](const boost::system::error_code& ec, size_t) {
    if(ec) {
      handle_connection_error("do_read_header", ec);
      return;
    }
    read_msg->decode_header();
    debug("got header\n");
    do_read_body();
  });
}

void Peer::do_read_body() {
  ba::async_read(sock, read_msg->get_raw(),
                 [this](const boost::system::error_code& ec, const size_t& bytes_read) {
    if(ec) return (size_t)0;
    return read_msg->get_body_size()-bytes_read;
  },
  [this](const bs::error_code& ec, const size_t&) {
    if(ec) {
      handle_connection_error("do_read_msg", ec);
      return;
    }
    read_msg->decode_body();

    Msg_ptr msg = read_msg;

    thread([this,msg]() {
      process_msg(msg);
    }).detach();
    do_read_header();
  });
}

// -------------- handlers --------------

void Peer::process_msg(Msg_ptr msg){
  debug("processing message..");
  switch(msg->get_type()){
  case Message::T_ECHO:
    handle_echo(msg);
    break;
  default:
    debug("unknown messape type");
  }
}

void Peer::handle_echo(const Msg_ptr &msg){
  string text = msg->get_string(Message::K_TEXT);
  printf("ECHO : %s\n", text.c_str());
}
