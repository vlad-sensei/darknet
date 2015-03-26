#ifndef CONNECTION_H
#define CONNECTION_H

#include "glob.h"
#include "connection_base.h"

class Connection;
typedef shared_ptr<Connection> Connection_ptr;
typedef Connection_base<Connection> Connection_network;

class Connection: public enable_shared_from_this<Connection>, Connection_network{
  Connection();
public:
  Connection(tcp::socket& socket):Connection_network(socket){}
  void init();
  inline Connection_ptr shared_from_this(){return enable_shared_from_this<Connection>::shared_from_this();}
  void process_msg(const Msg_ptr& msg);
};

#endif // CONNECTION_H
