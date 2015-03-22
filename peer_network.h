#ifndef PEER_NETWORK_H
#define PEER_NETWORK_H

#include <mutex>
#include <deque>
#include <memory>

#include <boost/asio.hpp>

#include "glob.h"
#include "message.h"

using tcp=boost::asio::ip::tcp;
namespace bs = boost::system;

class Peer;
typedef shared_ptr<Peer> Peer_ptr;

class Peer_network {
  Peer_network();
protected:
  Peer_network(tcp::socket& socket);
  virtual ~Peer_network(){}

  void run();
  void send(Msg_ptr msg);
  void close();

private:
  virtual void process_msg(const Msg_ptr& msg)=0;
  virtual void terminate() = 0;
  virtual Peer_ptr shared_from_this() = 0;

  void do_read_header();
  void do_read_body();
  void do_write();

  void handle_connection_error(const string& location, const string& error);

  tcp::socket socket_;
  Msg_ptr read_msg,write_msg;

  deque<Msg_ptr> msg_queue;
  mutex msg_write_mtx, socket_close_mtx;
  bool msg_is_writing = false;
  bool is_socket_closed = true;

};

#endif // PEER_NETWORK_H
