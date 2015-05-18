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


#include <boost/asio.hpp>

#include "glob.h"
#include "connection_initiator_base.h"
#include "message.h"
#include "peer.h"
#include "ui.h"
#include "library.h"


#define DEBUG

class Core;
typedef unique_ptr<Core> Core_ptr;

class Ui;
typedef unique_ptr<Ui> Ui_ptr;

extern Core_ptr core;

class Core : Connection_initiator_base, public Library {
public:
  Core();
  inline void set_daemon_port(const uint16_t& port){Connection_initiator_base::set_port(port);}
  inline uint16_t get_daemon_port() const {return Connection_initiator_base::get_port();}
  inline void set_client_port(const uint16_t& port){ui->set_port(port);}
  void run();
  bool remove_peer(const peer_id_t& pid);
  void ai_run();

  // user interaction (UI)
  void connect(const string& addr, const uint16_t& port);
  void broadcast_echo(const string& msg);
  //start syncing at regular intervalls
  void start_synch(int period);
  //stop syncing after the next syncing is completed
  void stop_synch();
  bool merge_peers(const peer_id_t& pid1, const peer_id_t& pid2);
  void share_peers(uint16_t max_count, const peer_id_t &pid);
  void set_database_path(const string path){Database::set_database_path(path);}
  bool make_peer_req(const peer_id_t &pid1);

  bool req_file(const Id &mid, Id &bid);
  bool req_file_from_peers(const Id &bid,const bool& aggresive=false);

  void handle_aggressive_query(const Id& bid,const unordered_set<Id>& cids,const peer_id_t& buyer_pid);
  void handle_chunk(const Id& bid, const Chunk& chunk);
  void handle_chunk_ack(const Id& bid, const unordered_set<Id>& cids, const peer_id_t& seller_pid);
  void handle_chunk_forward_ack(const Id& bid,const unordered_set<Id>& cids,const ip_t& addr);
  void add_file_req_timeout(const Id& bid, const time_t& timeout);

private:
  void req_chunks(const Id& bid, const unordered_set<Id>& cids);
  void synch_all();
  bool spawn_peer(socket_ptr& socket);
  void handle_new_connection(socket_ptr socket);



  //all data&methods in data must be synchronized
  struct {
    //peers
    peer_id_t current_peer_id = 0;
    unordered_map<peer_id_t,Peer_ptr> peers;
    unordered_map<ip_t, peer_id_t> peer_ips;
    inline bool peer_exists(const peer_id_t& pid) {return peers.find(pid)!=peers.end();}
    inline bool peer_ip_exists(const ip_t& ip_v4){return peer_ips.find(ip_v4)!=peer_ips.end();}

    //synch
    bool should_sync = SYNC;
    bool sync_thread_exists = false;
    thread sync_thread;

    //chunk requests
    map<time_t,Id>file_reqs_timeout;
    unordered_map<Id, File_req > file_reqs;
    unordered_map<Id, Inidirect_file_req > indirect_reqs;
    inline bool file_req_exists(const Id& bid){ return file_reqs.find(bid) != file_reqs.end();}
    inline bool chunk_req_exists(const Id& bid,const Id& cid){ return file_req_exists(bid) && file_reqs[bid].chunk_exists(cid);}

    inline bool indirect_file_req_exists(const Id& bid){ return indirect_reqs.find(bid) != indirect_reqs.end();}
    inline bool indirect_chunk_req_exists(const Id& bid,const Id& cid){ return indirect_file_req_exists(bid) && indirect_reqs[bid].chunk_exists(cid);}

  } data;
  rw_mutex peers_mtx, pid_mtx, sync_mtx, chunk_req_mtx,timeout_mtx;

  Ui_ptr ui;
};

#endif // CORE_H
