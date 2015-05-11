#ifndef PEER_H
#define PEER_H

/*
 * Peer
 *
 * Peer represents the peer we are connected to and
 * provides relevant API such as send function and
 * various message handlers. Obviously it also contains
 * data about our peer, such as ID.
 *
 * Every Peer object has an open tcp socket connected
 * to the actual peer, and lives only as long as that
 * socket is open.
 */


#include <memory>
#include <thread>
#include <mutex>
#include <deque>

#include <boost/asio.hpp>

#include "glob.h"
#include "message.h"
#include "connection_base.h"

namespace bs = boost::system;
typedef boost::asio::ip::tcp::socket socket_t;

class Peer;
typedef shared_ptr<Peer> Peer_ptr;
typedef Connection_base<Peer> Peer_connection;

class Peer: public enable_shared_from_this<Peer>, Peer_connection {
public:
  Peer(socket_ptr& sock_, const peer_id_t& pid);
  ~Peer();
  void init();
  inline void echo(const string& echo_msg = "echo_msg") {send(Message::echo(echo_msg));}
  inline void req_chunks(const Id& bid, const unordered_set<Id>& cids) {send(Message::chunk_req(bid,cids));}
  inline void req_metaheads(){send(Message::meta_req());}
  inline void send_metaheads(const vector<Metahead>& metaheads){send(Message::meta_list(metaheads));}
  inline void req_peers(){
    send(Message::peer_req(DEAFULT_PEER_REQ_COUNT));
  }
  inline void send_listen_port(const uint16_t& port){send(Message::port(port));}
  inline void merge_peer(const ip_t& ip, const uint16_t& port){send(Message::adress(ip,port));}

  inline ip_t get_ip() const {return remote_ip;}
  bool get_listen_port(uint16_t& listen_port);
private:
  Peer();
  void terminate();
  inline Peer_ptr shared_from_this(){return enable_shared_from_this<Peer>::shared_from_this();}

  //data
  void set_listen_port(const uint16_t& listen_port);


  //process messages
  void process_msg(const Msg_ptr& msg);
  void handle_echo(const Msg_ptr& msg);
  void handle_chunk_req(const Msg_ptr& msg);
  void handle_meta_req(const Msg_ptr& msg);
  void handle_meta_reply(const Msg_ptr& msg);
  void handle_chunk(const Msg_ptr& msg);
  void handle_listen_port(const Msg_ptr& msg);
  void handle_connect(const Msg_ptr &msg);
  void handle_peer_req(const Msg_ptr &msg);

  struct Data{
    Data(const peer_id_t& pid_):pid(pid_){}
    const peer_id_t pid;
    uint16_t listen_port = 0;
  private:
    Data();
  } data;
  rw_mutex listen_port_mtx;
};

#endif // PEER_H
