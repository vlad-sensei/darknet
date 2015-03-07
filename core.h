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
#include <random>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>

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

typedef shared_ptr<ba::io_service> io_service_ptr;
typedef shared_ptr<tcp::socket> socket_ptr;
typedef shared_ptr<tcp::acceptor> acceptor_ptr;

extern Core_ptr core;

class Core : public Library, public UI {
public:
  Core(ba::io_service& io_service,tcp::endpoint& endpoint);
  // user interaction
  void connect(const string& addr, const uint16_t& port);
  void broadcast_echo(const string& msg);

  void spawn_peer(tcp::socket& socket,const peer_id_t& pid);
  void remove_peer(const peer_id_t& pid);
  void broadcast(Msg_ptr msg);
  peer_id_t get_pid();

private:
  Core();

  //network
  void do_accept();
  void do_connect(const string&addr, const uint16_t& port);
  void do_verify_peer(tcp::socket socket);

  ba::io_service& io_service_;
  tcp::acceptor acceptor_;
  tcp::socket socket_;
  tcp::resolver resolver_;


  //all data&methods in data must be synchronized
  struct {
    peer_id_t get_id();
    //TODO: introduce id system propely
    peer_id_t pid = 0;
    unordered_map<peer_id_t,Peer_ptr> peers;
    shared_timed_mutex peers_mtx, pid_mtx;

    bool peer_exists(const peer_id_t& pid) {return peers.find(pid)!=peers.end();}
  } data;

};

#endif // CORE_H
