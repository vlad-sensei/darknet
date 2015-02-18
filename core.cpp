#include "core.h"

Core_ptr core;

Core::Core(boost::asio::io_service &io_service, tcp::endpoint &endpoint):
  acceptor(io_service, endpoint), acceptor_socket(io_service){
  debug("creating core..\n");
  lib.reset(new Library);
  ui.reset(new UI);

  do_accept();
}

void Core::do_accept(){
  acceptor.async_accept(acceptor_socket,
                        [this](const bs::error_code& ec){
    if(!ec) do_handle(move(acceptor_socket));
    do_accept();
  });
}

void Core::do_handle(tcp::socket sock){
  debug("got new connection..\n");
  //Add peer here
}
