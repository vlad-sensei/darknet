#include "connection_initiator_base.h"
#include <csignal>
#include <utility>
#include <thread>

Connection_initiator_base::Connection_initiator_base():
  io_service_(),
  signals_(io_service_),
  acceptor_(io_service_),
  resolver_(io_service_),
  client_context_(ba::ssl::context::tlsv12_client),
  server_context_(ba::ssl::context::tlsv12_server)
{
  debug("creating connection_initiator..");
  await_stop();
  //debug("CERT:\n%s", TMP_CERT);
  //server_context_.use_certificate_chain_file("/ram/cert.pem");
  server_context_.use_certificate_chain(ba::const_buffer(TMP_CERT.c_str(),TMP_CERT.size()));
  server_context_.use_rsa_private_key(ba::const_buffer(TMP_KEY.c_str(), TMP_KEY.size()), ba::ssl::context::pem);
  //server_context_.use_private_key_file("/ram/key.pem", ba::ssl::context::pem);
  //context_.set_options(ba::ssl::context::default_workarounds |
   //                        ba::ssl::context::no_sslv2);
                          // ba::ssl::context::single_dh_use);
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
   // exit(0);
  });
}

void Connection_initiator_base::listen(){
  accept_socket_ = socket_ptr(new ssl_socket_t(io_service_, server_context_));
  acceptor_.async_accept(accept_socket_->lowest_layer(),
                         [this](const bs::error_code& ec){
    if(!acceptor_.is_open()) return;
    if(!ec){
      socket_ptr new_socket = accept_socket_;
      new_socket->async_handshake(ba::ssl::stream_base::server, [this, new_socket](const bs::error_code& ec){
        if(ec) {
          debug(" *** tls handshake failed [%s]", ec.message());
          return;
        }
        handle_new_connection(new_socket);
      });
    }
    listen();
  });
}


void Connection_initiator_base::connect(const string &addr, const uint16_t &port){
  thread([this, addr, port](){
    socket_ptr s = socket_ptr(new ssl_socket_t(io_service_, client_context_));
    try{
      const tcp::resolver::query& q = tcp::resolver::query(addr,to_string(port));
      ba::connect(s->lowest_layer(),resolver_.resolve(q));
      s->handshake(ba::ssl::stream_base::client);
    }catch(exception&e){
      debug(" *** error in do_connect : %s",e.what());
      return;
    }
    handle_new_connection(s);
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
