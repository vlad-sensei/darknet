#ifndef CORE_H
#define CORE_H

/*
 * Core
 *
 * Core is the core class of the program.
 * It is resposible for high-level program
 * and control of its subclasses. It also
 * listens for incoming connections in order
 * to spawn more Peer objects.
 */

#include <memory>
#include <thread>

#include <boost/asio.hpp>

#include "glob.h"
#include "message.h"
#include "peer.h"
#include "ui.h"
#include "library.h"

using tcp = boost::asio::ip::tcp;
namespace ba = boost::asio;
namespace bs = boost::system;

class Core;
typedef unique_ptr<Core> Core_ptr;
class Peer;
typedef shared_ptr<Peer> Peer_ptr;
class UI;
typedef unique_ptr<UI> UI_ptr;

extern Core_ptr core;

class Core
{
public:
  Core(ba::io_service& io_service, tcp::endpoint& endpoint);

  //public methods must be threadsafe!
  class Data{
  public:
    //data calls
  private:
    //actual data&mutexes

  } data;

private:
  Core();

  //network
  void do_accept();
  void do_handle(tcp::socket sock);

  tcp::acceptor acceptor;
  tcp::socket acceptor_socket;

  UI_ptr ui;
  Library_ptr lib;

};

#endif // CORE_H
