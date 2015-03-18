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
#include "core_network.h"
#include "message.h"
#include "peer.h"
#include "ui.h"
#include "library.h"

class Core;
typedef unique_ptr<Core> Core_ptr;

typedef shared_ptr<ba::io_service> io_service_ptr;
typedef shared_ptr<tcp::socket> socket_ptr;
typedef shared_ptr<tcp::acceptor> acceptor_ptr;

extern Core_ptr core;

class Core : public Core_network, public Library, public UI {
public:
  Core(){}
  void run();
  // user interaction
  void connect(const string& addr, const uint16_t& port);
  void broadcast_echo(const string& msg);

  void spawn_peer(tcp::socket& socket);
  bool remove_peer(const peer_id_t& pid);
  void broadcast(Msg_ptr msg);
  peer_id_t get_pid();

private:
  void verify_new_connection(tcp::socket socket);


  //all data&methods in data must be synchronized
  struct {
    peer_id_t current_peer_id = 0;
    unordered_map<peer_id_t,Peer_ptr> peers;
    inline bool peer_exists(const peer_id_t& pid) {return peers.find(pid)!=peers.end();}
  } data;
  rw_mutex peers_mtx, pid_mtx;

};

#endif // CORE_H
