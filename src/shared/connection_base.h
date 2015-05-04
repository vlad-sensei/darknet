#ifndef CONNECTION_BASE_H
#define CONNECTION_BASE_H

#include <mutex>
#include <deque>
#include <memory>
#include <thread>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "glob.h"
#include "message.h"

using tcp=boost::asio::ip::tcp;
namespace bs = boost::system;

typedef ba::ssl::stream<tcp::socket> ssl_socket_t;
typedef shared_ptr<ssl_socket_t> socket_ptr;

template<typename Tderived>
class Connection_base {
  Connection_base();
protected:
  Connection_base(socket_ptr& socket);
  virtual ~Connection_base(){}

  void run();
  void send(Msg_ptr msg);
  void close();

  const ip_t remote_ip;
private:
  virtual void process_msg(const Msg_ptr& msg)=0;
  virtual void terminate(){}
  virtual shared_ptr<Tderived> shared_from_this() = 0;

  void do_read_header();
  void do_read_body();
  void do_write();

  void handle_connection_error(const string& location, const string& error);

  socket_ptr socket_;
  Msg_ptr read_msg,write_msg;

  deque<Msg_ptr> msg_queue;
  mutex msg_write_mtx, socket_close_mtx;
  bool msg_is_writing = false;
  bool is_socket_closed = true;
};

template<typename Tderived> inline
Connection_base<Tderived>::Connection_base(socket_ptr& socket):
  remote_ip(socket->lowest_layer().remote_endpoint().address().to_v4().to_ulong()),
  socket_(move(socket))
{}

template<typename Tderived> inline
void Connection_base<Tderived>::run(){
  do_read_header();
}

template<typename Tderived> inline
void Connection_base<Tderived>::do_read_header(){
  read_msg = Message::empty();
  auto self = shared_from_this();
  ba::async_read(*socket_, read_msg->get_raw(),[this, self](const bs::error_code& ec, const size_t&bytes)->size_t{
    if(ec) return 0;
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


template<typename Tderived> inline
void Connection_base<Tderived>::do_read_body(){
  auto self = shared_from_this();
  ba::async_read(*socket_, read_msg->get_raw(),[this,self](const bs::error_code& ec, const size_t&bytes)->size_t{
    if(ec) return 0;
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

template<typename Tderived> inline
void Connection_base<Tderived>::send(Msg_ptr msg){
  unique_lock<mutex> lck(msg_write_mtx);
  msg_queue.push_back(msg);
  lck.unlock();
  do_write();
}

template<typename Tderived> inline
void Connection_base<Tderived>::do_write(){
  unique_lock<mutex> lck(msg_write_mtx);
  if(msg_queue.empty()|| msg_is_writing) return;
  msg_is_writing = true;
  write_msg = msg_queue.front();
  msg_queue.pop_front();
  lck.unlock();
  auto self = shared_from_this();
  ba::async_write(*socket_, write_msg->get_raw(), [this,self](const bs::error_code &ec, const size_t&){
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

template<typename Tderived> inline
void Connection_base<Tderived>::handle_connection_error(const string &location, const string &error){
  debug(" *** connection error in %s : %s", location.c_str(), error.c_str());
  close();
}

template<typename Tderived> inline
void Connection_base<Tderived>::close(){
  unique_lock<mutex> lck(socket_close_mtx);
  if(!socket_->lowest_layer().is_open()) return;
  socket_->lowest_layer().close();
  lck.unlock();
  terminate();
}

#endif // CONNECTION_BASE_H
