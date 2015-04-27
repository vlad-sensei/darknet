#include "connection_initiator_base.h"
#include <csignal>
#include <utility>
#include <thread>

Connection_initiator_base::Connection_initiator_base():
  io_service_(),
  signals_(io_service_),
  accept_socket_(io_service_),
  acceptor_(io_service_),
  resolver_(io_service_)
{
  debug("creating connection_initiator..");
  await_stop();
}

void Connection_initiator_base::run(){
  io_service_.run();
}

void Connection_initiator_base::stop(){
  io_service_.stop();
}


void Connection_initiator_base::start_listen(){
  debug("listening to port %s..", get_port());
  tcp::endpoint endpoint(tcp::v4(),get_port()); //*resolver_.resolve({"localhost", to_string(get_port())});
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  listen();
}

void Connection_initiator_base::await_stop(){
  signals_.async_wait([this](const bs::error_code&, const int& signo){
    debug("got signal %d", signo);
    acceptor_.close();
    io_service_.stop();
    exit(0);
  });
}

void Connection_initiator_base::listen(){
  acceptor_.async_accept(accept_socket_,
                         [this](const bs::error_code& ec){
    if(!acceptor_.is_open()) return;
    if(!ec)
      handle_new_connection(move(accept_socket_));
    listen();
  });
}


void Connection_initiator_base::connect(const string &addr, const uint16_t &port){
  thread([this, addr, port](){
    tcp::socket s(io_service_);
    try{
      const tcp::resolver::query& q = tcp::resolver::query(addr,to_string(port));
      ba::connect(s,resolver_.resolve(q));
    }catch(exception&e){
      debug(" *** error in do_connect : %s",e.what());
      return;
    }
    handle_new_connection(move(s));
  }).detach();
}

void Connection_initiator_base::set_port(const uint16_t &port){
  w_lock l(port_mtx);
  data.port = port;
}

uint16_t Connection_initiator_base::get_port() const{
 r_lock l(port_mtx);
 return data.port;
}
