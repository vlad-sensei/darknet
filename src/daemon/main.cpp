#include <sstream>

#include "glob.h"
#include "core.h"

using namespace std;

bool process_args(int argc, char* argv[], uint16_t& port){
  if(argc>2){
    printf("./darknet [PORT]\n");
    return false;
  }
  port = DEFAULT_LISTEN_PORT;
  if(argc==2) {
    istringstream ss(argv[1]);
    if(!(ss >> port)){
      printf("./darknet [PORT]\n");
      return false;
    }
  }
  return true;
}

int main(int argc, char* argv[]){
  try{
    uint16_t port = DEFAULT_LISTEN_PORT;
    if(!process_args(argc, argv,port)) return 0;
    //debug("listening at port %u", port);
    core.reset(new Core);
    core->set_port(port);
    //thread([&](){core->run_test_uploader();}).detach();
    core->run();
  } catch(std::exception& e){
    debug(" *** exception : %s", e.what());
  }
  debug("terminated");

  return 0;
}

