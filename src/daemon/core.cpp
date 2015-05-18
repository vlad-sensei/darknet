#include "core.h"
#include <ratio>
Core_ptr core;

// -------- Constructors ----
Core::Core() : ui(unique_ptr<Ui>(new Ui)){}

void Core::run(){
  thread Inventory_thread([this](){
    init_inventory();
  });
  thread Database_thread([this](){
    init_db();
  });

  Inventory_thread.join();
  Database_thread.join();

  thread network_thread([this](){
    Connection_initiator_base::start_listen();
    Connection_initiator_base::run();
  });
  thread ui_thread([this](){
    ui->run();
  });

  thread daemon_ai([this](){
    core->ai_run();
  });

  network_thread.join();
  ui_thread.join();
}

void Core::ai_run(){
  while(true){
    time_t now = time(0);

    w_lock timeout_lck(timeout_mtx);

    //handle all timed out file queries
    while(data.file_reqs_timeout.begin()->first<=now){
      Id bid = data.file_reqs_timeout.begin()->second;
      data.file_reqs_timeout.erase(data.file_reqs_timeout.begin());
      //handle the timeout request..


    }


    for(auto iter = data.file_reqs_time.begin(); iter != data.file_reqs_time.end(); ++iter){

      const time_t& time_stamp=iter->first;
      Id bid=iter->second;

      if(difftime(time_now,time_stamp) < DEFAULT_WAIT_TO_AGGRESIV) {
        debug("*** no more old querys [diff %s] [size %s]", difftime(time_now,time_stamp),data.file_reqs_time.size());
        break;
      }
      // make an aggresiv query if old and stil wanted
      if(data.file_req_exists(bid)){
        req_file_from_peers(iter->second,true);
        data.file_reqs_time[time_now]=bid;
      }
      data.file_reqs_time.erase(time_stamp);
    }


    time_lck.unlock();
    r_lock chunk_lck(chunk_req_mtx);
    r_lock peer_lck(peers_mtx);

    //req chunks from all peers that have left an ack
    for(auto& it:data.file_reqs){
      for(const auto& it2:it.second.chunks){
        if(it.second.get_peer_ip(it2.first,peer_ip)&& data.peer_ip_exists(peer_ip)){
          const Peer_ptr& peer =data.peers[data.peer_ips[peer_ip]];
          const unordered_set<Id> cids={it2.first};
          peer->req_chunks(it.first,cids);
        }
      }
    }


    chunk_lck.unlock();
    peer_lck.unlock();

    //        vector<Metahead> metahead_list=publish_metaheads();
    //        if(!metahead_list.empty() && rand()%100 < DEFUALT_PERCENT_TO_DOWNLOAD ){
    //            Metahead random=metahead_list[rand()%metahead_list.size()];
    //            req_file(random.mid,random.bid);
    //        }

    this_thread::sleep_for(chrono::seconds(DEFAULT_AI_SLEEP));
  }
}

void Core::connect(const string &addr, const uint16_t &port){
  debug("connecting [%s:%d]..",addr.c_str(),port);
  Connection_initiator_base::connect(addr,port);
}

void Core::broadcast_echo(const string &msg){
  r_lock l(peers_mtx);
  for(const auto& it:data.peers){
    const Peer_ptr& peer = it.second;
    peer->echo(msg);
  }
}


void Core::handle_new_connection(socket_ptr socket){
  debug("new peer [%s]..", socket->lowest_layer().remote_endpoint());
  //TODO:check if we are already connected by MAC/IP parameters
  if(!socket->lowest_layer().is_open()) {debug(" *** socket is not open"); return;}
  if(!socket->lowest_layer().remote_endpoint().address().is_v4()){
    debug(" *** only accepting ipv4 clients at this moment");
    return;
  }

  //maybe cache those in the future
  if(!spawn_peer(socket)) {debug(" *** could not spawn new peer");};
}

void Core::req_chunks(const Id &bid, const unordered_set<Id> &cids){
  debug("req_chunks");
  r_lock l(peers_mtx);
  for(const auto& it:data.peers){
    const Peer_ptr& peer = it.second;
    peer->req_chunks(bid, cids);
  }
}

//-------- syncing ----
//TODO fix starting after stopping
void Core::start_synch(int period){
  //Capturing this might lead to a dangling pointer if core is destroyed

  w_lock l(sync_mtx);
  data.should_sync = true;

  if (data.sync_thread_exists) return;
  data.sync_thread = thread([this, period](void){
    r_lock l(sync_mtx, defer_lock_type());
    while(true){
      this_thread::sleep_for(chrono::seconds(period));
      l.lock();
      if (data.should_sync){
        l.unlock();
        core->synch_all();
      } else l.unlock();
    }
  });
  data.sync_thread_exists = true;
}

void Core::stop_synch(){
  w_lock l(sync_mtx);
  data.should_sync = false;
}

void Core::synch_all(){
  r_lock l(peers_mtx);
  for(const auto& it:data.peers){
    const Peer_ptr& peer = it.second;
    peer->req_metaheads();
  }
}


// ----------- Routing ---------

bool Core::merge_peers(const peer_id_t &pid1, const peer_id_t &pid2){
  debug("merging peers [pid1 %s] [pid2 %s]",pid1,pid2);
  //verify they exist
  r_lock l(peers_mtx);
  if(!data.peer_exists(pid1)||!data.peer_exists(pid2)) {
    //debug("*** peers did not exist [%s] [%s]",pid1 ,pid2);
    return false;
  }
  const Peer_ptr& peer1 = data.peers[pid1];
  const Peer_ptr& peer2 = data.peers[pid2];
  uint16_t port1, port2;
  if(!peer1->get_listen_port(port1) && !peer2->get_listen_port(port2)) {
    //debug("*** no listen port from the peer",pid1 ,pid2);
    return false;
  }
  peer1->merge_peer(peer2->get_ip(),port2);
  peer2->merge_peer(peer1->get_ip(),port1);
  return true;
}

void Core::share_peers(uint16_t max_count,const peer_id_t& pid){
  int counter=0;
  r_lock l(peers_mtx);
  vector<peer_id_t> peer_list;
  for(const auto& iterator:data.peers){
    if (pid == iterator.first) continue;
    peer_list.emplace_back(iterator.first);
    if (++counter == max_count) break;
  }
  l.unlock();
  for(auto i:peer_list){
    core->merge_peers(i,pid);
  }
}

bool Core::make_peer_req(const peer_id_t &pid){
  r_lock l(peers_mtx);
  if(!data.peer_exists(pid)) {
    //debug("*** peers did not exist [%s] [%s]",pid1 ,pid2);
    return false;
  }
  const Peer_ptr& peer = data.peers[pid];
  peer->req_peers();
  return true;
}

// --------------- chunk requests ----------

bool Core::req_file(const Id& mid,Id& bid){
  Metahead metahead;
  w_lock chunk_req_lck(chunk_req_mtx);
  if(!get_metahead(mid,metahead)&& !data.file_req_exists(metahead.bid)){
    debug("*** no mid found [mid %s]\n or a req is on going",mid);
    return false;
  }
  //TODO: use time + timeout delay
  time_t time_now=time(0);
  debug("req_file with:\n[mid %s]\n[bid %s]\n[time_stamp %s]",mid,metahead.bid,time_now);

  bid=metahead.bid;
  data.file_reqs[bid]=File_req(bid,time_now);

  w_lock file_req_timeout_lck(timeout_mtx);
  data.file_reqs_timeout[data.file_reqs[bid].created_at+DEFAULT_FILE_REQ_TIMEOUT]=bid;
  file_req_timeout_lck.unlock();
  chunk_req_lck.unlock();
  debug("*** TODO: check if file is on local pc");
  return req_file_from_peers(bid);
}

bool Core::req_file_from_peers(const Id& bid, const bool& aggresive){
  r_lock chunk_lck(chunk_req_mtx);
  if(!data.file_req_exists(bid)) {
    debug("*** no request for this bid %s",bid);
    return false;
  }
  unordered_set<Id> cids;
  for(const auto& c:data.file_reqs[bid].chunks) cids.emplace(c.first);
  chunk_lck.unlock();
  r_lock peer_lck(peers_mtx);
  for(const auto& it:data.peers){
    const Peer_ptr& peer = it.second;
    peer->chunk_query(bid,cids,aggresive);
  }
  peer_lck.unlock();
  return true;
}

void Core::handle_aggressive_query(const Id& bid,const unordered_set<Id>& cids,const peer_id_t& buyer_pid){
  w_lock chunk_req_lck(chunk_req_mtx);
  if(!data.indirect_file_req_exists(bid)){
    data.indirect_reqs[bid]=Inidirect_file_req(time(0));
  }

  for(const Id& cid:cids) data.indirect_reqs[bid].chunks[cid].emplace(buyer_pid);

  chunk_req_lck.unlock();
  r_lock peer_lck(peers_mtx);
  for(const auto& it:data.peers){
    const Peer_ptr& peer= it.second;
    // do no ask the one that asks you
    if(peer->get_pid()==buyer_pid) continue;
    peer->chunk_query(bid,cids);
  }
  peer_lck.unlock();

}

void Core::handle_chunk_forward_ack(const Id &bid, const unordered_set<Id> &cids, const ip_t &addr){
  debug("chunk forward ack..");
  r_lock chunk_lck(chunk_req_mtx);
  r_lock peer_lck(peers_mtx);
  if(!data.file_req_exists(bid) || !data.peer_ip_exists(addr)) {
    debug("*** do not need this any more or no peer with that ip");
    return;
  }
  peer_lck.unlock();

  for(const Id& cid:cids){
    if(data.file_reqs[bid].chunk_exists(cid))
      if(!data.file_reqs[bid].add_peer(cid,addr))
        debug(" *** could not add peer to file_req");
  }

}


void Core::handle_chunk_ack(const Id& bid,const unordered_set<Id>& cids,const peer_id_t& seller_pid){
  r_lock chunk_lck(chunk_req_mtx);
  if(!data.file_req_exists(bid) && !data.indirect_file_req_exists(bid)) {
    debug("*** do not need this any more");
    return;
  }

  //local request
  if(data.file_req_exists(bid)){
    for(const Id& cid:cids){
      if(data.file_reqs[bid].chunk_exists(cid))
        if(!data.file_reqs[bid].add_peer(cid,data.peers[seller_pid]->get_ip()))
          debug("*** could not add peer to file_req");
    }
  }

  //remote request
  if(data.indirect_file_req_exists(bid)){
    //get all peers who are awaiting for the cids
    unordered_map<peer_id_t,unordered_set<Id> > peer_map;
    for(const Id& cid:cids){
      if(data.indirect_reqs[bid].chunk_exists(cid)){
        for(const peer_id_t& peer_id:data.indirect_reqs[bid].chunks[cid]){
          peer_map[peer_id].emplace(cid);
        }
      }
    }
    r_lock peer_lck(peers_mtx);
    if(!data.peer_exists(seller_pid)) return;
    const Peer_ptr& seller_peer= data.peers[seller_pid];

    for(const auto& it:peer_map){
      const peer_id_t& buyer_pid = it.first;
      if(!data.peer_exists(buyer_pid)) continue;
      const Peer_ptr& destination_peer= data.peers[buyer_pid];

      merge_peers(seller_pid,buyer_pid);
      data.indirect_reqs[bid].remove_peer(buyer_pid);
      destination_peer->forward_ack(bid,peer_map[buyer_pid],seller_peer->get_ip());
    }
  }
  chunk_lck.unlock();
}

void Core::handle_chunk(const Id& bid, const Chunk& chunk){
  w_lock l(chunk_req_mtx);
  if (!data.chunk_req_exists(bid,chunk.cid)){
    debug("there is no request for this:\n[bid %s]\n[chunk.cid %s]\n",bid,chunk.cid);
    return;
  }
  File_req& req = data.file_reqs[bid];
  ++req.writer_count;
  req.erase(chunk.cid);
  l.unlock();
  const bool& add_chunk_success = add_chunk(bid,chunk);
  l.lock();
  --req.writer_count;
  if(!add_chunk_success) return;

  if(!req.chunks.empty() || req.writer_count){
    //debug("*** have more to get from the network");
    return;
  }

  //build metabody OR we are done
  if(req.has_metabody){
    //we think we are done
    data.file_reqs.erase(bid);
    l.unlock();
    //get_file(bid,"TODO_get_name_of_file");
    debug("!!!Download complet!!!");
    return;
  }
  l.unlock();

  //try to build a metabody
  Metabody metabody;
  bool get_metabody_success = get_metabody(bid, metabody);
  l.lock();

  if(!get_metabody_success){
    debug("*** need more chunks for a metabody");
    l.lock();
    data.file_reqs[bid].insert(metabody.bid_next());
    l.unlock();
    req_file_from_peers(bid);
    return;
  }
  req.has_metabody = true;
  for(const Id& cid:metabody.cids){
    req.insert(cid);
  }
  l.unlock();
  req_file_from_peers(bid);

}
// ----------- Data -----------
//TODO: consider diabling/tweaking for testing
bool Core::spawn_peer(socket_ptr &socket){
  w_lock l(peers_mtx);
  const ip_t& ip = socket->lowest_layer().remote_endpoint().address().to_v4().to_ulong();
#ifndef ALLOW_PEER_FROM_SAME_IP
  //if(data.peer_ip_exists(ip)) return false;
#endif
  const peer_id_t&  pid = ++data.current_peer_id;
  data.peer_ips[ip]=pid;
  data.peers[pid] = Peer_ptr(new Peer(socket, pid));
  data.peers[pid]->init();
  return true;
}

bool Core::remove_peer(const peer_id_t &pid){
  w_lock l(peers_mtx);
  if(!data.peer_exists(pid)) return false;
#ifndef ALLOW_PEER_FROM_SAME_IP
  //data.peer_ips.erase(data.peers[pid]->get_ip());
#endif
  data.peers.erase(pid);
  return true;
}
