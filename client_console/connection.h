/*
 * Connection
 *
 * Connection relays commands between UI and daemon.
 * It does not execude anything itself.
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <thread>
#include <mutex>
#include <condition_variable>

#include "glob.h"
#include "connection_base.h"
#include "ui.h"

class Connection;
typedef shared_ptr<Connection> Connection_ptr;
typedef Connection_base<Connection> Connection_network;

class Connection: public enable_shared_from_this<Connection>, public Connection_network{
  Connection();
public:
  Connection(tcp::socket& socket):Connection_network(socket){}

  void init();

  inline void text_command(const string& cmd){send(Message::ui_text_command(cmd));}
private:
  inline Connection_ptr shared_from_this(){return enable_shared_from_this<Connection>::shared_from_this();}
  void process_msg(const Msg_ptr& msg);
  void handle_echo(const Msg_ptr& msg);
/*
public:
  mutex m;
  condition_variable cv;
  */
};
#endif // CONNECTION_H
