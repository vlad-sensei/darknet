#include "core.h"

Core_ptr core;

Core::Core(boost::asio::io_service &io_service, tcp::endpoint &endpoint):
  io_service_(io_service),
  acceptor_(io_service,endpoint),
  socket_(io_service),
  resolver_(io_service),
  data(new Data){
  debug("creating core..");

  lib.reset(new Library);
  ui.reset(new UI);
  do_accept();
}

// -------- user interaction ----

void Core::connect(const string &addr, const uint16_t &port){
  debug("connecting [%s:%d]..",addr.c_str(),port);
  do_connect(addr,port);
}

void Core::broadcast_echo(const string &msg){
  data->broadcast(Message::echo(msg));
}


// ------- network -----------

void Core::do_accept(){
  debug("waiting for connection..");
  acceptor_.async_accept(socket_,
                         [this](const bs::error_code& ec){
    if(!ec){
        do_verify_peer(move(socket_));
    }
    do_accept();
  });
}

void Core::do_connect(const string &addr, const uint16_t &port){
  thread([this, addr, port](){
    tcp::socket s(io_service_);
    try{
      const tcp::resolver::query& q = tcp::resolver::query(addr,to_string(port));
      ba::connect(s,resolver_.resolve(q));
    }catch(exception&e){
      debug(" *** error in do_connect : %s",e.what());
      return;
    }
    do_verify_peer(move(s));
  }).detach();
}

void Core::do_verify_peer(tcp::socket socket){
  //best to do trollchecks before creating&alloctating an object
  debug("verifying peer..");
  peer_id_t pid;
  bool success = true;
  try{
    thread read_thread([&](){
      Msg_ptr read_msg(new Message);
      ba::read(socket, read_msg->get_raw(), [](const bs::error_code& ec, const size_t& bytes){
        if(ec) return 0UL;
        return Message::HEADER_SIZE-bytes;
      });
      read_msg->decode_header();
      ba::read(socket, read_msg->get_raw(),[&read_msg](const bs::error_code& ec, const size_t& bytes){
        if(ec) return 0UL;
        return read_msg->get_payload_size()-bytes;
      });
      read_msg->decode_msg();
      pid = read_msg->get_peer_id_t(Message::K_PEER_ID);
    });

    thread write_thread([&](){
      ba::write(socket, Message::verify(data->get_id())->get_raw());
    });
    read_thread.join();
    write_thread.join();
  } catch(exception&e){
    debug(" *** error : %s", e.what());
    success = false;
  }
  if(!success){
    debug("verification failed");
    return;
  }
  debug("other id : %llu", uint64_t(pid));
  data->spawn_peer(socket, pid);
}

// ----------- Data -----------

Core::Data::Data(){
  debug("creating data..");
  //TODO: do properly
  srand(time(0));
  id = rand();
  debug("id : %llu",uint64_t(id));
}

peer_id_t Core::Data::get_id(){
  return id;
}

void Core::Data::spawn_peer(tcp::socket &socket, const peer_id_t &pid){
  unique_lock<shared_timed_mutex> l(peers_mtx);
  if(has_peer_u(pid)){
    debug(" *** peer already exists [pid:%llu]",uint64_t(pid));
    return;
  }
  Peer_ptr peer(new Peer(socket, pid));
  peers[pid]=peer;
}

void Core::Data::remove_peer(const peer_id_t &pid){
  unique_lock<shared_timed_mutex> l(peers_mtx);
  if(!has_peer_u(pid)){
    debug(" *** can't remove peer since none exists");
    return;
  }
  peers.erase(pid);
}

void Core::Data::broadcast(Msg_ptr msg){
  shared_lock<shared_timed_mutex> l(peers_mtx);
  for(const auto& it:peers){
    const Peer_ptr& peer = it.second;
    peer->send(Message::copy_msg(msg));
  }
}

// ---------- data  -- unsyched ----

bool Core::Data::has_peer_u(const peer_id_t &pid){
  return peers.find(pid)!=peers.end();
}

