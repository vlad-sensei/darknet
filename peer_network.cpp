#include "peer_network.h"
#include <thread>

Peer_network::Peer_network(tcp::socket &socket): socket_(move(socket)){}

void Peer_network::run(){
  do_read_header();
}

void Peer_network::do_read_header(){
  read_msg = Message::empty();
  auto self = shared_from_this();
  ba::async_read(socket_, read_msg->get_raw(),[this, self](const bs::error_code& ec, const size_t&bytes){
    if(ec) return 0UL;
    return Message::HEADER_SIZE-bytes;
  },
  [this,self](const bs::error_code& ec, const size_t&){
    if(ec){
      handle_connection_error("do_read_header", ec.message());
      return;
    }
    read_msg->decode_header();
    do_read_body();
  });
}


void Peer_network::do_read_body(){
  auto self = shared_from_this();
  ba::async_read(socket_, read_msg->get_raw(),[this,self](const bs::error_code& ec, const size_t&bytes){
    if(ec) return 0UL;
    return read_msg->get_body_size()-bytes;
  },
  [this,self](const bs::error_code& ec, const size_t&){
    if(ec){
      handle_connection_error("do_read_body", ec.message());
      return;
    }
    read_msg->decode_body();
    Msg_ptr msg = read_msg;
    thread([this, msg](){process_msg(msg);}).detach();
    do_read_header();
  });
}

void Peer_network::send(Msg_ptr msg){
  unique_lock<mutex> lck(msg_write_mtx);
  msg_queue.push_back(msg);
  lck.unlock();
  do_write();
}

void Peer_network::do_write(){
  unique_lock<mutex> lck(msg_write_mtx);
  if(msg_queue.empty()|| msg_is_writing) return;
  msg_is_writing = true;
  write_msg = msg_queue.front();
  msg_queue.pop_front();
  lck.unlock();
  auto self = shared_from_this();
  ba::async_write(socket_, write_msg->get_raw(), [this,self](const bs::error_code &ec, const size_t&){
    if(ec){
      handle_connection_error("do_write", ec.message());
      return;
    }
    unique_lock<mutex> l(msg_write_mtx);
    msg_is_writing=false;
    write_msg.reset();
    l.unlock();
    do_write();
  });
}

void Peer_network::handle_connection_error(const string &location, const string &error){
  debug(" *** connection error in %s : %s", location.c_str(), error.c_str());
  close();
}

void Peer_network::close(){
  unique_lock<mutex> lck(socket_close_mtx);
  if(!socket_.is_open()) return;
  socket_.close();
  lck.unlock();
  terminate();
}
