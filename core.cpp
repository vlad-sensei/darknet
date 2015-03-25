#include "core.h"
#include <ratio>

Core_ptr core;

// -------- user interaction ----

void Core::run(){
  Core::run_network();
}

void Core::connect(const string &addr, const uint16_t &port){
  debug("connecting [%s:%d]..",addr.c_str(),port);
  Core_network::connect(addr,port);
}

void Core::broadcast_echo(const string &msg){
  r_lock l(peers_mtx);
  for(const auto& it:data.peers){
    const Peer_ptr& peer = it.second;
    peer->echo(msg);
  }
}


void Core::verify_new_connection(tcp::socket socket){
  debug("new peer..");
  //TODO:check if we are already connected by MAC/IP parameters
  //maybe cache those in the future
  spawn_peer(socket);
}

void Core::req_chunks(const Id &bid, const unordered_set<Id> &cids){
  r_lock l(peers_mtx);
  for(const auto& it:data.peers){
    const Peer_ptr& peer = it.second;
    peer->req_chunks(bid, cids);
  }
}


void Core::start_synch(){
    //Capturing this might lead to a dangling pointer if core is destroyed
    thread th( [this](void){
        debug("synching with all");
        synch_all();
        std::chrono::seconds sec(2);
        std::chrono::duration<int, ratio<1,1>> dur(sec);
        this_thread::sleep_for(dur);
    });
    th.detach();
}

void Core::synch_all(){
  r_lock l(peers_mtx);
  for(const auto& it:data.peers){
    const Peer_ptr& peer = it.second;
    peer->synch();
  }
}

// ----------- Data -----------
void Core::spawn_peer(tcp::socket &socket){
  w_lock l(peers_mtx);
  const peer_id_t&  pid = ++data.current_peer_id;
  data.peers[pid] = Peer_ptr(new Peer(socket, pid));
  data.peers[pid]->init();
}

bool Core::remove_peer(const peer_id_t &pid){
  w_lock l(peers_mtx);
  if(!data.peer_exists(pid)) return false;
  data.peers.erase(pid);
  return true;
}
