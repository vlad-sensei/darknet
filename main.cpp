#include "glob.h"
#include "core.h"

using namespace std;

int main(){
  try{
    ba::io_service io_s;
    tcp::endpoint endpoint(tcp::v4(),SERVER_PORT);
    core.reset(new Core(io_s, endpoint));
    io_s.run();
  } catch(std::exception& e){
    cerr << "**** Exception "<< e.what() << "\n";
  }
  debug("terminated\n");

  return 0;
}

