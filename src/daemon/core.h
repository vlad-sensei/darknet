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
#include "connection_initiator_base.h"
#include "message.h"
#include "peer.h"
#include "ui.h"
#include "library.h"

class Core;
typedef unique_ptr<Core> Core_ptr;
class UI;
typedef unique_ptr<UI> UI_ptr;

extern Core_ptr core;

class Core : Connection_initiator_base, public Library {
public:
  Core(){}
  inline void set_port(const uint16_t& port){Connection_initiator_base::set_port(port);}
  void run();
  bool remove_peer(const peer_id_t& pid);

  // user interaction (UI)
  void connect(const string& addr, const uint16_t& port);
  void broadcast_echo(const string& msg);

private:
  void req_chunks(const Id& bid, const unordered_set<Id>& cids);
  //start syncing at regular intervalls
  static void synch_loop();
  void start_synch();
  //synchronize with all peers
  void synch_all();
  void spawn_peer(tcp::socket& socket);
  void handle_new_connection(tcp::socket socket);

  //all data&methods in data must be synchronized
  struct {
    peer_id_t current_peer_id = 0;
    unordered_map<peer_id_t,Peer_ptr> peers;
    inline bool peer_exists(const peer_id_t& pid) {return peers.find(pid)!=peers.end();}
  } data;
  rw_mutex peers_mtx, pid_mtx;

  UI_ptr ui;
};

#endif // CORE_H
