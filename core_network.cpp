#include "core_network.h"
#include <csignal>
#include <utility>
#include <thread>

Core_network::Core_network():
  io_service_(),
  signals_(io_service_),
  accept_socket_(io_service_),
  acceptor_(io_service_),
  resolver_(io_service_)
{
  debug("creating core_network..");
  await_stop();
}

void Core_network::run_network(){
  tcp::endpoint endpoint = *resolver_.resolve({"localhost", to_string(get_port())});
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  listen();
  io_service_.run();
}

void Core_network::await_stop(){
  signals_.async_wait([this](const bs::error_code&, const int& signo){
    debug("got signal %d", signo);
    acceptor_.close();
    io_service_.stop();
    exit(0);
  });
}

void Core_network::listen(){
  acceptor_.async_accept(accept_socket_,
                         [this](const bs::error_code& ec){
    if(!acceptor_.is_open()) return;
    if(!ec)
      verify_new_connection(move(accept_socket_));
    listen();
  });
}


void Core_network::connect(const string &addr, const uint16_t &port){
  thread([this, addr, port](){
    tcp::socket s(io_service_);
    try{
      const tcp::resolver::query& q = tcp::resolver::query(addr,to_string(port));
      ba::connect(s,resolver_.resolve(q));
    }catch(exception&e){
      debug(" *** error in do_connect : %s",e.what());
      return;
    }
    verify_new_connection(move(s));
  }).detach();
}

void Core_network::set_port(const uint16_t &port){
  w_lock l(port_mtx);
  data.port = port;
}

uint16_t Core_network::get_port(){
 r_lock l(port_mtx);
 return data.port;
}
