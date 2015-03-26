#ifndef CORE_NETWORK_H
#define CORE_NETWORK_H

#include <memory>
#include <boost/asio.hpp>

#include "glob.h"

using tcp = boost::asio::ip::tcp;
namespace ba = boost::asio;
namespace bs = boost::system;

typedef shared_ptr<tcp::socket> socket_ptr;

class Network_initiator_base {
public:
  void set_port(const uint16_t& port);
  void run();
protected:
  Network_initiator_base();
  virtual ~Network_initiator_base(){}

  void connect(const string& addr, const uint16_t& port);

private:
  virtual void verify_new_connection(tcp::socket socket) = 0;

  void await_stop();
  void listen();
  uint16_t get_port();

//maybe a bit overkill, but best to keep it standard
  struct Data {
    uint16_t port = DEFAULT_LISTEN_PORT;
  } data;
  rw_mutex port_mtx;

  ba::io_service io_service_;
  ba::signal_set signals_;
  tcp::socket accept_socket_;
  tcp::acceptor acceptor_;
  tcp::resolver resolver_;
};

#endif // CORE_NETWORK_H
